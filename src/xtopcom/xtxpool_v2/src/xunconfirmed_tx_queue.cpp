// Copyright (c) 2017-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xtxpool_v2/xunconfirmed_tx_queue.h"

#include "xdata/xblocktool.h"
#include "xmbus/xevent_behind.h"
#include "xtxpool_v2/xtxpool_error.h"
#include "xtxpool_v2/xtxpool_log.h"
#include "xtxpool_v2/xreceipt_state_cache.h"

#include <unordered_set>

NS_BEG2(top, xtxpool_v2)

#define resend_time_threshold (60)

void xpeer_table_unconfirmed_txs_t::push_tx(const xcons_transaction_ptr_t & tx) {
    auto it = m_unconfirmed_txs.find(tx->get_last_action_receipt_id());
    if (it != m_unconfirmed_txs.end()) {
        return;
    }
    m_unconfirmed_txs[tx->get_last_action_receipt_id()] = tx;
}

void xpeer_table_unconfirmed_txs_t::erase(uint64_t receipt_id, xall_unconfirm_tx_set_t & all_unconfirm_tx_set) {
    auto it_unconfirmed_txs = m_unconfirmed_txs.find(receipt_id);
    if (it_unconfirmed_txs == m_unconfirmed_txs.end()) {
        return;
    }

    auto & tx = it_unconfirmed_txs->second;

    xtxpool_info("xpeer_table_unconfirmed_txs_t::erase tx:%s", tx->dump(true).c_str());

    auto range = all_unconfirm_tx_set.equal_range(tx);
    for (auto it_range = range.first; it_range != range.second; it_range++) {
        if (it_range->get()->get_transaction()->get_digest_str() == tx->get_transaction()->get_digest_str()) {
            all_unconfirm_tx_set.erase(it_range);
            break;
        }
    }

    m_unconfirmed_txs.erase(it_unconfirmed_txs);
}

const xcons_transaction_ptr_t xpeer_table_unconfirmed_txs_t::find(uint64_t receipt_id) const {
    auto it = m_unconfirmed_txs.find(receipt_id);
    if (it != m_unconfirmed_txs.end()) {
        return it->second;
    }
    return nullptr;
}

void xpeer_table_unconfirmed_txs_t::update_receipt_id(uint64_t latest_id, xall_unconfirm_tx_set_t & all_unconfirm_tx_set) {
    if (m_latest_receipt_id >= latest_id) {
        return;
    }

    uint64_t front_receipt_id = m_unconfirmed_txs.begin()->first;
    for (uint64_t id = front_receipt_id; id <= latest_id; id++) {
        erase(id, all_unconfirm_tx_set);
    }
    m_latest_receipt_id = latest_id;
}

void xpeer_tables_t::push_tx(const xcons_transaction_ptr_t & tx) {
    base::xvaccount_t vaccount(tx->get_target_addr());
    auto peer_table_sid = vaccount.get_short_table_id();
    xtxpool_info("xpeer_tables_t::push_tx tx:%s,peer table sid:%d,receipt id:%llu", tx->dump(true).c_str(), peer_table_sid, tx->get_last_action_receipt_id());
    auto it = m_peer_tables.find(peer_table_sid);
    if (it != m_peer_tables.end()) {
        it->second->push_tx(tx);
    } else {
        auto peer_table = std::make_shared<xpeer_table_unconfirmed_txs_t>();
        peer_table->push_tx(tx);
        m_peer_tables[peer_table_sid] = peer_table;
    }
    m_all_unconfirm_txs.insert(tx);
}

void xpeer_tables_t::erase(uint16_t peer_table_sid, uint64_t receipt_id) {
    auto it = m_peer_tables.find(peer_table_sid);
    if (it != m_peer_tables.end()) {
        it->second->erase(receipt_id, m_all_unconfirm_txs);
    }
}

const xcons_transaction_ptr_t xpeer_tables_t::find(uint16_t peer_table_sid, uint64_t receipt_id) const {
    auto it = m_peer_tables.find(peer_table_sid);
    xtxpool_dbg("xpeer_tables_t::find peer table sid:%d,receipt id:%llu", peer_table_sid, receipt_id);
    if (it != m_peer_tables.end()) {
        return it->second->find(receipt_id);
    }
    return nullptr;
}

void xpeer_tables_t::update_receiptid_state(const xreceipt_state_cache_t & receiptid_state_cache) {
    for (auto peer_table : m_peer_tables) {
        auto & peer_table_sid = peer_table.first;
        auto & peer_table_txs = peer_table.second;
        peer_table_txs->update_receipt_id(receiptid_state_cache.get_confirmid_max(peer_table_sid), m_all_unconfirm_txs);
    }

    // update latest and max receipt id of peer tables, if not found, create one, if anyone have no unconfirmed tx, remove it.
    // auto & id_state_map = id_state.get_map();
    // for (auto & peer_table_id_state_pair : id_state_map) {
    //     auto & peer_table_sid = peer_table_id_state_pair.first;
    //     auto & peer_table_id_state = peer_table_id_state_pair.second;
    //     auto it_peer_table_unconfirmed_txs = m_peer_tables.find(peer_table_sid);
    //     if (it_peer_table_unconfirmed_txs != m_peer_tables.end()) {
    //         it_peer_table_unconfirmed_txs->second->update_receipt_id(peer_table_id_state->get_latest_commit_out_id(), peer_table_id_state->get_max_out_id(),
    //         m_all_unconfirm_txs); if (it_peer_table_unconfirmed_txs->second->is_all_txs_confirmed()) {
    //             m_peer_tables.erase(it_peer_table_unconfirmed_txs);
    //         }
    //     } else {
    //         if (!peer_table_id_state->all_confirmed()) {
    //             auto peer_table = std::make_shared<xpeer_table_unconfirmed_txs_t>();
    //             peer_table->update_receipt_id(peer_table_id_state->get_latest_commit_out_id(), peer_table_id_state->get_max_out_id(), m_all_unconfirm_txs);
    //             m_peer_tables[peer_table_sid] = peer_table;
    //         }
    //     }
    // }
}

int32_t xunconfirmed_account_t::update(xblock_t * latest_committed_block, const xreceipt_state_cache_t & receiptid_state_cache) {
    std::unordered_set<std::string> confirm_txs;
    std::map<std::string, xcons_transaction_ptr_t> unconfirmed_txs;
    auto account_addr = latest_committed_block->get_account();
    uint64_t latest_height = latest_committed_block->get_height();

    for (uint64_t cur_height = latest_height; cur_height > m_highest_height; cur_height--) {
        auto _block = m_para->get_vblockstore()->load_block_object(account_addr, cur_height, base::enum_xvblock_flag_committed, true);
        if (_block == nullptr) {
            base::xauto_ptr<base::xvblock_t> _block_ptr = m_para->get_vblockstore()->get_latest_connected_block(account_addr);
            uint64_t start_sync_height = _block_ptr->get_height() + 1;
            xtxpool_info("xunconfirmed_account_t::update account:%s state fall behind,try sync unit from:%llu,end:%llu", account_addr.c_str(), start_sync_height, cur_height);
            if (cur_height > start_sync_height) {
                uint32_t sync_num = (uint32_t)(cur_height - start_sync_height);
                mbus::xevent_behind_ptr_t ev = make_object_ptr<mbus::xevent_behind_on_demand_t>(account_addr, start_sync_height, sync_num, true, "unit_lack");
                m_para->get_bus()->push_event(ev);
            }
            return xtxpool_error_unitblock_lack;
        }

        data::xblock_t * unit_block = dynamic_cast<data::xblock_t *>(_block.get());
        // check if finish load block
        if (unit_block->is_genesis_block() || (unit_block->get_block_class() == base::enum_xvblock_class_full) ||
            (unit_block->get_block_class() == base::enum_xvblock_class_light && unit_block->get_unconfirm_sendtx_num() == 0)) {
            confirm_txs.clear();
            for (auto & tx_info : m_unconfirmed_txs) {
                // clear confirmed tx receiptid
                m_peer_tables->erase(tx_info.second->get_peer_table_sid(), tx_info.second->get_receipt_id());
                xtxpool_info("xunconfirmed_account_t::update unconfirm pop tx,account=%s,tx=%s", account_addr.c_str(), base::xstring_utl::to_hex(tx_info.first).c_str());
            }
            m_unconfirmed_txs.clear();
            break;
        }

        // nil block do nothing
        if (unit_block->get_block_class() == base::enum_xvblock_class_nil) {
            continue;
        }

        // only light unit and has unconfirm sendtx, should update unconfirm cache
        data::xlightunit_block_t * lightunit = dynamic_cast<data::xlightunit_block_t *>(unit_block);
        for (auto & tx : lightunit->get_txs()) {
            std::string hash_str = tx->get_tx_hash();
            if (tx->is_send_tx()) {
                auto result = confirm_txs.find(hash_str);
                if (result != confirm_txs.end()) {
                    confirm_txs.erase(result);
                } else {
                    auto recv_tx = lightunit->create_one_txreceipt(tx->get_raw_tx().get());
                    unconfirmed_txs[hash_str] = recv_tx;
                }
                continue;
            }

            if (tx->is_confirm_tx()) {
                confirm_txs.insert(hash_str);
                continue;
            }
        }
    }

    for (auto & hash : confirm_txs) {
        auto it = m_unconfirmed_txs.find(hash);
        if (it != m_unconfirmed_txs.end()) {
            m_peer_tables->erase(it->second->get_peer_table_sid(), it->second->get_receipt_id());
            m_unconfirmed_txs.erase(it);
            xtxpool_info("xunconfirmed_account_t::update unconfirm pop tx,account=%s,tx=%s", account_addr.c_str(), base::xstring_utl::to_hex(hash).c_str());
        }
    }

    if (latest_height > m_highest_height) {
        m_highest_height = latest_height;
    }

    for (auto & it_s : unconfirmed_txs) {
        auto & tx = it_s.second;
        base::xvaccount_t vaccount(tx->get_target_addr());
        auto peer_table_sid = vaccount.get_short_table_id();
        auto confirmid_max = receiptid_state_cache.get_confirmid_max(peer_table_sid);
        if (tx->get_last_action_receipt_id() <= confirmid_max) {
            xtxpool_info("xunconfirmed_account_t::update account:%s tx:%s already confirmed", account_addr.c_str(), tx->dump(true).c_str());
            continue;
        }
        xtxpool_info("xunconfirmed_account_t::update unconfirm push tx,account=%s,tx=%s", account_addr.c_str(), tx->dump().c_str());
        auto tx_info = std::make_shared<xunconfirmed_tx_info_t>(peer_table_sid, tx->get_last_action_receipt_id());
        m_unconfirmed_txs[it_s.first] = tx_info;
        m_peer_tables->push_tx(tx);
    }

    return xtxpool_success;
}

const xcons_transaction_ptr_t xunconfirmed_account_t::find(const uint256_t & hash) const {
    std::string hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
    auto it = m_unconfirmed_txs.find(hash_str);
    if (it != m_unconfirmed_txs.end()) {
        auto & unconfirmed_tx_info = it->second;
        return m_peer_tables->find(unconfirmed_tx_info->get_peer_table_sid(), unconfirmed_tx_info->get_receipt_id());
    }
    return nullptr;
}

void xunconfirmed_tx_queue_t::udpate_latest_confirmed_block(xblock_t * block, const xreceipt_state_cache_t & receiptid_state_cache) {
    xtxpool_dbg("xunconfirmed_tx_queue_t::udpate_latest_confirmed_block block:%s", block->dump().c_str());
    auto it_account = m_unconfirmed_accounts.find(block->get_account());
    if (it_account != m_unconfirmed_accounts.end()) {
        auto & unconfirmed_account = it_account->second;
        unconfirmed_account->update(block, receiptid_state_cache);
        if (unconfirmed_account->size() == 0) {
            xtxpool_dbg("xunconfirmed_tx_queue_t::udpate_latest_confirmed_block unconfirm pop account,block=%s", block->dump().c_str());
            m_unconfirmed_accounts.erase(it_account);
        }
    } else {
        auto unconfirmed_account = std::make_shared<xunconfirmed_account_t>(m_para, &m_peer_tables);
        unconfirmed_account->update(block, receiptid_state_cache);
        if (unconfirmed_account->size() != 0) {
            xtxpool_dbg("xunconfirmed_tx_queue_t::udpate_latest_confirmed_block unconfirm push account,block=%s", block->dump().c_str());
            m_unconfirmed_accounts[block->get_account()] = unconfirmed_account;
        }
    }
}

const xcons_transaction_ptr_t xunconfirmed_tx_queue_t::find(const std::string & account_addr, const uint256_t & hash) const {
    auto it_account = m_unconfirmed_accounts.find(account_addr);
    if (it_account != m_unconfirmed_accounts.end()) {
        auto & unconfirmed_account = it_account->second;
        return unconfirmed_account->find(hash);
    }
    return nullptr;
}

void xunconfirmed_tx_queue_t::recover(const xreceipt_state_cache_t & receiptid_state_cache) {
    xtxpool_info("xunconfirmed_tx_queue_t::recover table=%s", m_table_info->get_table_addr().c_str());
    // update table-table receipt id state, if all peer_tables were complate, no need to recover, or else, get unconfirmed accounts, and load their unconfirmed txs.
    m_peer_tables.update_receiptid_state(receiptid_state_cache);
    // receiptid_state may not be consistance with account state!
    // if (m_peer_tables.is_complated()) {
    //     xtxpool_dbg("xunconfirmed_tx_queue_t::recover table:%s, all peer tables unconfirmed txs is complate, no need to recover.", m_table_info->get_table_addr().c_str());
    //     return;
    // }

    base::xauto_ptr<xblockchain2_t> blockchain{m_para->get_store()->clone_account(m_table_info->get_table_addr())};
    if (blockchain != nullptr) {
        std::set<std::string> accounts = blockchain->get_unconfirmed_accounts();
        if (!accounts.empty()) {
            xtxpool_info("xunconfirmed_tx_queue_t::recover unconfirmed accounts not empty size:%u", accounts.size());

            // remove unconfirmed accounts which not found from accounts.
            for (auto it_unconfirmed_account = m_unconfirmed_accounts.begin(); it_unconfirmed_account != m_unconfirmed_accounts.end();) {
                auto & account_addr = it_unconfirmed_account->first;
                auto it_accounts = accounts.find(account_addr);
                if (it_accounts == accounts.end()) {
                    xtxpool_info("xunconfirmed_tx_queue_t::recover unconfirm pop account=%s", account_addr.c_str());
                    it_unconfirmed_account = m_unconfirmed_accounts.erase(it_unconfirmed_account);
                } else {
                    it_unconfirmed_account++;
                }
            }

            // recover unconfirmed txs.
            for (auto & account : accounts) {
                xtxpool_info("xunconfirmed_tx_queue_t::recover account:%s", account.c_str());
                base::xauto_ptr<base::xvblock_t> unitblock = m_para->get_vblockstore()->get_latest_committed_block(account);
                if (unitblock != nullptr) {
                    xblock_t * block = dynamic_cast<xblock_t *>(unitblock.get());
                    udpate_latest_confirmed_block(block, receiptid_state_cache);
                }
            }
        }
    }
}

const std::vector<xcons_transaction_ptr_t> xunconfirmed_tx_queue_t::get_resend_txs(uint64_t now) {
    std::vector<xcons_transaction_ptr_t> resend_txs;
    auto & all_txs = m_peer_tables.get_all_txs();
    for (auto tx : all_txs) {
        if (tx->get_unit_cert()->get_gmtime() + resend_time_threshold > now) {
            break;
        }
        resend_txs.push_back(tx);
    }
    return resend_txs;
}

uint32_t xunconfirmed_tx_queue_t::size() const {
    return m_peer_tables.get_all_txs().size();
}

NS_END2
