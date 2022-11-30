// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xtxpool_v2/xtxpool_table.h"

#include "xbasic/xmodule_type.h"
#include "xdata/xblocktool.h"
#include "xdata/xlightunit.h"
#include "xdata/xtable_bstate.h"
#include "xmbus/xevent_behind.h"
#include "xtxpool_v2/xnon_ready_account.h"
#include "xtxpool_v2/xtxpool_error.h"
#include "xtxpool_v2/xtxpool_log.h"
#include "xverifier/xtx_verifier.h"
#include "xverifier/xverifier_errors.h"
#include "xverifier/xverifier_utl.h"
#include "xverifier/xwhitelist_verifier.h"
#include "xvledger/xvblock_fork.h"
#include "xvledger/xvblockbuild.h"
#include "xvledger/xvcontract.h"
#include "xvledger/xvledger.h"
#include "xbase/xutl.h"
#include "xstatestore/xstatestore_face.h"

namespace top {
namespace xtxpool_v2 {

#define state_update_too_long_time (600)
#define load_table_block_num_max (20)
#define table_fail_behind_height_diff_max (5)
#define table_sync_on_demand_num_max (10)

bool xtxpool_table_t::is_reach_limit(const std::shared_ptr<xtx_entry> & tx) const {
    if (tx->get_tx()->is_send_tx()) {
        auto peer_table_sid = tx->get_tx()->get_peer_tableid();
        if (m_table_state_cache.is_unconfirmed_num_reach_limit(peer_table_sid)) {
            xtxpool_warn("xtxpool_table_t::push_send_tx table-table unconfirm txs reached upper limit tx:%s,peer_sid:%d", tx->get_tx()->dump().c_str(), peer_table_sid);
            return true;
        }
    }
    return false;
}

int32_t xtxpool_table_t::push_send_tx_real(const std::shared_ptr<xtx_entry> & tx) {
    uint64_t latest_nonce;
    // bool is_cached_nonce = false;

    auto & account_addr = tx->get_tx()->get_source_addr();

    // {
    //     std::lock_guard<std::mutex> lck(m_mgr_mutex);
    //     is_cached_nonce = m_txmgr_table.get_account_nonce_cache(account_addr, latest_nonce);
    // }

    // if (!is_cached_nonce) {
    bool result = get_account_latest_nonce(account_addr, latest_nonce);
    if (!result) {
        // todo : push to non_ready_accounts
        // std::lock_guard<std::mutex> lck(m_non_ready_mutex);
        // m_non_ready_accounts.push_tx(tx);
        xtxpool_warn("xtxpool_table_t::push_send_tx account state fall behind tx:%s", tx->get_tx()->dump(true).c_str());
        return xtxpool_error_account_state_fall_behind;
    }
    // }

    if (data::is_sys_contract_address(common::xaccount_address_t{account_addr})) {
        tx->get_para().set_tx_type_score(enum_xtx_type_socre_system);
    } else {
        tx->get_para().set_tx_type_score(enum_xtx_type_socre_normal);
    }

    int32_t ret;
    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        // if (!is_cached_nonce) {
        m_txmgr_table.updata_latest_nonce(account_addr, latest_nonce);
        // }
        ret = m_txmgr_table.push_send_tx(tx, latest_nonce);
    }
    if (ret != xsuccess) {
        // XMETRICS_COUNTER_INCREMENT("txpool_push_tx_send_fail", 1);
        m_xtable_info.get_statistic()->inc_push_tx_send_fail_num(1);
    }
    return ret;
}

int32_t xtxpool_table_t::push_send_tx(const std::shared_ptr<xtx_entry> & tx) {
    if (is_reach_limit(tx)) {
        return xtxpool_error_account_unconfirm_txs_reached_upper_limit;
    }

    int32_t ret = verify_send_tx(tx->get_tx(), true);
    if (ret != xsuccess) {
        return ret;
    }
    ret = push_send_tx_real(tx);
    if (ret == xsuccess) {
        XMETRICS_COUNTER_INCREMENT(m_push_send_tx_metrics_name, 1);
    }
    return ret;
}

int32_t xtxpool_table_t::push_receipt_real(const std::shared_ptr<xtx_entry> & tx) {
    // auto & account_addr = tx->get_tx()->get_account_addr();

    // store::xaccount_basic_info_t account_basic_info;
    // bool result = m_table_indexstore->get_account_basic_info(account_addr, account_basic_info);
    // if (!result) {
    //     // todo : push to non_ready_accounts
    //     // std::lock_guard<std::mutex> lck(m_non_ready_mutex);
    //     // m_non_ready_accounts.push_tx(tx);
    //     xtxpool_warn("xtxpool_table_t::push_receipt account state fall behind tx:%s", tx->get_tx()->dump(true).c_str());
    //     return xtxpool_error_account_state_fall_behind;
    // }

    if (data::is_sys_contract_address(common::xaccount_address_t{tx->get_tx()->get_account_addr()})) {
        tx->get_para().set_tx_type_score(enum_xtx_type_socre_system);
    } else {
        tx->get_para().set_tx_type_score(enum_xtx_type_socre_normal);
    }

    int32_t ret;
    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        ret = m_txmgr_table.push_receipt(tx);
    }
    if (ret != xsuccess) {
        // XMETRICS_COUNTER_INCREMENT("txpool_push_tx_receipt_fail", 1);
        m_xtable_info.get_statistic()->inc_push_tx_receipt_fail_num(1);
    } else {
        // TODO(jimmy) mpt and all states will be synced together delete future
        if (tx->get_tx()->is_recv_tx()) {
            auto account_addr = tx->get_tx()->get_account_addr();
            base::xaccount_index_t account_index;
            bool ret2 = m_table_state_cache.get_account_index(account_addr, account_index);
            if (ret2) {
                if (account_index.get_latest_unit_hash().empty()) {  // TODO(jimmy) only for old version accountindex delete future
                    base::xvaccount_t _account_vaddress(account_addr);
                    uint64_t latest_connect_height = m_para->get_vblockstore()->get_latest_connected_block_height(_account_vaddress);
                    xblocktool_t::check_lacking_unit_and_try_sync(_account_vaddress, account_index, latest_connect_height, m_para->get_vblockstore(), "txpool");
                }
            } else {
                xtxpool_warn("xtxpool_table_t::push_receipt_real get account index fail account:%s", account_addr.c_str());
            }
        }
    }

    return ret;
}

int32_t xtxpool_table_t::push_receipt(const std::shared_ptr<xtx_entry> & tx, bool is_self_send) {
    if (tx->get_tx()->is_recv_tx()) {
        m_unconfirm_id_height.update_peer_confirm_id(tx->get_tx()->get_peer_tableid(), tx->get_tx()->get_last_action_sender_confirmed_receipt_id());
    }

    uint64_t latest_receipt_id = m_table_state_cache.get_tx_corresponding_latest_receipt_id(tx);
    uint64_t tx_receipt_id = tx->get_tx()->get_last_action_receipt_id();
    if (tx_receipt_id < latest_receipt_id) {
        xtxpool_warn("xtxpool_table_t::push_receipt duplicate receipt:%s,id:%llu:%llu", tx->get_tx()->dump().c_str(), tx_receipt_id, latest_receipt_id);
        // XMETRICS_COUNTER_INCREMENT("txpool_receipt_duplicate", 1);
        m_xtable_info.get_statistic()->inc_receipt_duplicate_num(1);
        return xtxpool_error_tx_duplicate;
    }

    // check if receipt is repeat before verify it, because receipt verify is a time-consuming job.
    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        bool is_repeat = m_txmgr_table.is_repeat_tx(tx);
        if (is_repeat) {
            xtxpool_warn("xtxpool_table_t::push_receipt repeat receipt:%s", tx->get_tx()->dump().c_str());
            // XMETRICS_COUNTER_INCREMENT("txpool_receipt_repeat", 1);
            m_xtable_info.get_statistic()->inc_receipt_repeat_num(1);
            return xtxpool_error_request_tx_repeat;
        }
    }
    if (!is_self_send) {
        int32_t ret = verify_receipt_tx(tx->get_tx());
        if (ret != xsuccess) {
            return ret;
        }
    }

    return push_receipt_real(tx);
}

xcons_transaction_ptr_t xtxpool_table_t::pop_tx(const tx_info_t & txinfo, bool clear_follower) {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    return m_txmgr_table.pop_tx(txinfo.get_hash_str(), txinfo.get_subtype(), clear_follower);
}

void xtxpool_table_t::update_id_state(const std::vector<update_id_state_para> & para_vec) {
    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        for (auto & para : para_vec) {
            m_txmgr_table.update_id_state(para.m_txinfo, para.m_peer_table_sid, para.m_receiptid, para.m_nonce);
        }
    }

    // std::lock_guard<std::mutex> lck(m_non_ready_mutex);
    // m_non_ready_accounts.pop_tx(txinfo);
}

xpack_resource xtxpool_table_t::get_pack_resource(const xtxs_pack_para_t & pack_para) {
    std::vector<xcons_transaction_ptr_t> txs;
    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        txs = m_txmgr_table.get_ready_txs(pack_para, m_unconfirm_id_height);
    }

    if (txs.empty()) {
        return {};
    }

    filter_txs_by_black_white_list(txs);
    if (txs.empty()) {
        return {};
    }
    auto self_sid = m_xtable_info.get_short_table_id();

    // if a peer table already have confirm tx tobe packed, do not use receipt id state to modify coressponding confirm id.
    std::set<base::xtable_shortid_t> peer_sids_for_confirm_id = pack_para.get_peer_sids_for_confirm_id();
    std::map<base::xtable_shortid_t, xreceiptid_state_and_prove> receiptid_state_prove_map;
    if (peer_sids_for_confirm_id.empty()){
        return xpack_resource(txs, receiptid_state_prove_map);
    }
    for (auto & tx : txs) {
        if (tx->is_confirm_tx()) {
            xdbg("xpack_resource xtxpool_table_t::get_pack_resource confirm tx:%s", tx->dump().c_str());
            auto it = peer_sids_for_confirm_id.find(tx->get_peer_tableid());
            if (it != peer_sids_for_confirm_id.end()) {
                xdbg("xpack_resource xtxpool_table_t::get_pack_resource not check peer sid confirm id self:%d,peer:%d", self_sid, tx->get_peer_tableid());
                peer_sids_for_confirm_id.erase(it);
            }
        }
    }

    auto & self_receiptid_state = pack_para.get_table_state_highqc()->get_receiptid_state();
    for (auto & peer_sid : peer_sids_for_confirm_id) {
        base::xreceiptid_pair_t self_pair;
        self_receiptid_state->find_pair(peer_sid, self_pair);
        uint64_t confirmid_max = self_pair.get_confirmid_max();
        uint64_t max_not_need_confirm_receiptid = self_pair.get_sendid_max();

        if (self_pair.all_confirmed_as_sender() && confirmid_max < max_not_need_confirm_receiptid) {
            auto receiptid_state_prove =
                m_para->get_receiptid_state_cache().get_receiptid_state_and_prove(self_sid, peer_sid, confirmid_max + 1, max_not_need_confirm_receiptid);
            if (receiptid_state_prove.m_property_prove_ptr != nullptr && receiptid_state_prove.m_receiptid_state != nullptr) {
                receiptid_state_prove_map[peer_sid] = receiptid_state_prove;
            }
        }
    }

    return xpack_resource(txs, receiptid_state_prove_map);
}

xcons_transaction_ptr_t xtxpool_table_t::query_tx(const std::string & account, const uint256_t & hash) {
    std::string hash_str = std::string(reinterpret_cast<char *>(hash.data()), hash.size());
    return query_tx(account, hash_str);
}

xcons_transaction_ptr_t xtxpool_table_t::query_tx(const std::string & account, const std::string & hash_str) {
    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        auto tx = m_txmgr_table.query_tx(account, hash_str);
        if (tx != nullptr) {
            return tx;
        }
    }

    return m_uncommit_txs.query_tx(hash_str);
}

void xtxpool_table_t::updata_latest_nonce(const std::string & account_addr, uint64_t latest_nonce) {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    return m_txmgr_table.updata_latest_nonce(account_addr, latest_nonce);
}

// bool xtxpool_table_t::is_account_need_update(const std::string & account_addr) const {
//     std::lock_guard<std::mutex> lck(m_mgr_mutex);
//     return m_txmgr_table.is_account_need_update(account_addr);
// }

void xtxpool_table_t::deal_commit_table_block(xblock_t * table_block, bool update_txmgr) {
    base::xvaccount_t _vaccount(table_block->get_account());
    if (false == base::xvchain_t::instance().get_xblockstore()->load_block_input(_vaccount, table_block, metrics::blockstore_access_from_txpool_refresh_table)) {
        xerror("xtxpool_table_t::deal_commit_table_block fail-load block input output, block=%s", table_block->dump().c_str());
        return;
    }

    std::vector<xtx_id_height_info> tx_id_height_infos;
    std::vector<xraw_tx_info> raw_txs;

    std::vector<update_id_state_para> update_id_state_para_vec;
    auto tx_actions = data::xblockextract_t::unpack_txactions(table_block);

    xdbg("xtxpool_table_t::deal_commit_table_block table block:%s", table_block->dump().c_str());

    for (auto & txaction : tx_actions) {
        bool need_confirm = !txaction.get_not_need_confirm();
        uint64_t txnonce = 0;
        if (txaction.get_tx_subtype() == base::enum_transaction_subtype_send || txaction.get_tx_subtype() == base::enum_transaction_subtype_self) {
            xtransaction_ptr_t _rawtx = table_block->query_raw_transaction(txaction.get_tx_hash());
            if (_rawtx != nullptr) {
                txnonce = _rawtx->get_tx_nonce();
                xinfo("xtxpool_table_t::deal_commit_table_block loaded rawtx:%s", _rawtx->dump().c_str());
                if (txaction.get_tx_subtype() == base::enum_transaction_subtype_send && need_confirm) {
                    if (!txaction.get_inner_table_flag()) {
                        raw_txs.push_back(xraw_tx_info(txaction.get_receipt_id_peer_tableid(), txaction.get_receipt_id(), _rawtx));
                    } else {
                        xdbg("xtxpool_table_t::deal_commit_table_block no need rawtx:%s", _rawtx->dump().c_str());
                    }
                }
            } else {
                xtxpool_error("xtxpool_table_t::deal_commit_table_block get raw tx fail table:%s,peer table:%d,tx type:%d,receipt_id::%llu,confirm id:%llu,table_height:%llu",
                             m_xtable_info.get_account().c_str(),
                             txaction.get_receipt_id_peer_tableid(),
                             txaction.get_tx_subtype(),
                             txaction.get_receipt_id(),
                             txaction.get_sender_confirmed_receipt_id(),
                             table_block->get_height());
            }
        }
        if (update_txmgr) {
            const std::string uri = txaction.get_contract_uri();
            const std::string & account = base::xvcontract_t::get_contract_address(uri);
            tx_info_t txinfo(account, txaction.get_tx_hash_256(), txaction.get_tx_subtype());
            update_id_state_para_vec.push_back(update_id_state_para(txinfo, txaction.get_receipt_id_peer_tableid(), txaction.get_receipt_id(), txnonce));
        }

        if (txaction.get_tx_subtype() == base::enum_transaction_subtype_send || txaction.get_tx_subtype() == base::enum_transaction_subtype_recv) {
            if (false == txaction.get_inner_table_flag()) {
                tx_id_height_infos.push_back(xtx_id_height_info(txaction.get_tx_subtype(), txaction.get_receipt_id_peer_tableid(), txaction.get_receipt_id(), need_confirm));

                xtxpool_info(
                    "xtxpool_table_t::deal_commit_table_block update unconfirm id height table:%s,peer table:%d,tx type:%d,receipt_id:%llu,confirm "
                    "id:%llu,table_height:%llu,need_confirm:%d",
                    m_xtable_info.get_account().c_str(),
                    txaction.get_receipt_id_peer_tableid(),
                    txaction.get_tx_subtype(),
                    txaction.get_receipt_id(),
                    txaction.get_sender_confirmed_receipt_id(),
                    table_block->get_height(),
                    need_confirm);
            } else {
                xdbg("xtxpool_table_t::deal_commit_table_block no id height tx=%s", base::xstring_utl::to_hex(txaction.get_org_tx_hash()).c_str());
            }
        }
    }

    if (update_txmgr) {
        update_id_state(update_id_state_para_vec);
    }
    m_unconfirm_id_height.update_unconfirm_id_height(table_block->get_height(), table_block->get_cert()->get_gmtime(), tx_id_height_infos);
    m_unconfirm_raw_txs.add_raw_txs(raw_txs);
}

void xtxpool_table_t::on_block_confirmed(xblock_t * table_block) {
    deal_commit_table_block(table_block, true);
}

int32_t xtxpool_table_t::verify_txs(const std::string & account, const std::vector<xcons_transaction_ptr_t> & txs) {
    for (auto & tx : txs) {
        auto tx_inside = query_tx(tx->get_account_addr(), tx->get_tx_hash());
        if (tx_inside != nullptr) {
            if (tx_inside->get_tx_subtype() == tx->get_tx_subtype()) {
                continue;
            } else if (tx_inside->get_tx_subtype() > tx->get_tx_subtype()) {
                return xtxpool_error_request_tx_repeat;
            }
        }
        int32_t ret = verify_cons_tx(tx);
        if (ret != xsuccess) {
            xtxpool_warn("xtxpool_table_t::verify_txs verify fail,tx:%s,err:%u", tx->dump(true).c_str(), ret);
            if (ret == xverifier::xverifier_error::xverifier_error_tx_duration_expired) {
                return xsuccess;
            }
            return ret;
        }

        xtxpool_v2::xtx_para_t para;
        std::shared_ptr<xtxpool_v2::xtx_entry> tx_ent = std::make_shared<xtxpool_v2::xtx_entry>(tx, para);
        if (tx->is_send_tx() || tx->is_self_tx()) {
            if (!is_reach_limit(tx_ent)) {
                xtxpool_info("xtxpool_table_t::verify_txs push tx from proposal tx:%s", tx->dump().c_str());
                XMETRICS_GAUGE(metrics::txpool_push_tx_from_proposal, 1);
                push_send_tx_real(tx_ent);
            }
        } else {
            uint64_t latest_receipt_id = m_table_state_cache.get_tx_corresponding_latest_receipt_id(tx_ent);
            uint64_t tx_receipt_id = tx->get_last_action_receipt_id();
            if (tx_receipt_id < latest_receipt_id) {
                xtxpool_warn("xtxpool_table_t::push_receipt duplicate receipt:%s,id:%llu:%llu", tx->dump().c_str(), tx_receipt_id, latest_receipt_id);
                return xtxpool_error_tx_duplicate;
            } else {
                xtxpool_info("xtxpool_table_t::verify_txs push tx from proposal tx:%s", tx->dump().c_str());
                XMETRICS_GAUGE(metrics::txpool_push_tx_from_proposal, 1);
                push_receipt_real(tx_ent);
            }
        }
    }
    return xsuccess;
}

void xtxpool_table_t::refresh_table(bool refresh_state_only) {
    auto latest_committed_block = base::xvchain_t::instance().get_xblockstore()->get_latest_committed_block(m_xtable_info, metrics::blockstore_access_from_txpool_refresh_table);
    xtxpool_dbg("xtxpool_table_t::refresh_table begin table:%s,commit_height:%llu", m_xtable_info.get_account().c_str(), latest_committed_block->get_height());

    uint64_t old_state_height = m_table_state_cache.get_state_height();
    if (old_state_height < latest_committed_block()->get_height()) {
        data::xtablestate_ptr_t tablestate = statestore::xstatestore_hub_t::instance()->get_table_state_by_block(latest_committed_block.get());
        if (tablestate == nullptr) {
            xtxpool_warn("xtxpool_table_t::refresh_table fail-get bstate.table=%s,block=%s", m_xtable_info.get_table_addr().c_str(), latest_committed_block->dump().c_str());
        } else {
            update_table_state(tablestate);
        }
    }

    if (refresh_state_only) {
        return;
    }

    auto receiptid_state = m_para->get_receiptid_state_cache().get_table_receiptid_state(m_xtable_info.get_short_table_id());
    if (receiptid_state != nullptr) {
        m_unconfirm_raw_txs.refresh(receiptid_state);
    }

    {
        std::lock_guard<std::mutex> lck(m_mgr_mutex);
        if (receiptid_state != nullptr) {
            m_txmgr_table.update_receiptid_state(receiptid_state);
        }
        m_txmgr_table.clear_expired_txs();
    }

    uint64_t left_end = 0;
    uint64_t right_end = 0;
    bool ret = m_unconfirm_id_height.get_lacking_section(m_para->get_receiptid_state_cache(), m_xtable_info.get_all_table_sids(), left_end, right_end, load_table_block_num_max);
    if (ret) {
        uint64_t load_height_max = right_end;
        uint64_t load_height_min = left_end;
        if (left_end == 0 && right_end == 0) {
            if (latest_committed_block->get_height() <= 1) {
                xtxpool_warn("xtxpool_table_t::refresh_table load commit block fail,table:%s", m_xtable_info.get_account().c_str());
                return;
            }
            deal_commit_table_block(dynamic_cast<xblock_t *>(latest_committed_block.get()), false);
            load_height_max = latest_committed_block->get_height() - 1;
            load_height_min = (load_height_max >= load_table_block_num_max) ? (load_height_max + 1 - load_table_block_num_max) : 1;
        }

        for (uint64_t height = load_height_max; height >= load_height_min; height--) {
            base::xauto_ptr<base::xvblock_t> commit_block =
                m_para->get_vblockstore()->load_block_object(m_xtable_info, height, base::enum_xvblock_flag_committed, false, metrics::blockstore_access_from_txpool_refresh_table);

            if (commit_block != nullptr) {
                deal_commit_table_block(dynamic_cast<xblock_t *>(commit_block.get()), false);
            } else {
                uint64_t sync_from_height = (height > load_height_min + table_sync_on_demand_num_max - 1) ? (height - table_sync_on_demand_num_max + 1) : load_height_min;
                // try sync table block
                mbus::xevent_behind_ptr_t ev = make_object_ptr<mbus::xevent_behind_on_demand_t>(
                    m_xtable_info.get_account(), sync_from_height, (uint32_t)(height - sync_from_height + 1), false, "lack_of_table_block", "", false);
                m_para->get_bus()->push_event(ev);
                xtxpool_warn("xtxpool_table_t::refresh_table load table block fail:table:%s,try sync %llu-%llu", m_xtable_info.get_account().c_str(), sync_from_height, height);
                XMETRICS_GAUGE(metrics::txpool_try_sync_table_block, 1);
                break;
            }
        }
    }
    xtxpool_info("xtxpool_table_t::refresh_table finish table:%s,old_state_height:%llu,new_state_height=%llu,get_lacking_ret=%d,left_end=%ld,right_end=%ld",
                 m_xtable_info.get_account().c_str(),
                 old_state_height,
                 m_table_state_cache.get_state_height(),
                 ret,
                 left_end,
                 right_end);
}

// void xtxpool_table_t::update_non_ready_accounts() {
// std::vector<std::shared_ptr<xtx_entry>> repush_txs;
// {
//     std::lock_guard<std::mutex> lck(m_non_ready_mutex);
//     auto account_addrs = m_non_ready_accounts.get_accounts();
//     for (auto & account_addr : account_addrs) {
//         store::xaccount_basic_info_t account_basic_info;
//         bool result = m_table_indexstore->get_account_basic_info(account_addr, account_basic_info);
//         if (result) {
//             auto tx_ents = m_non_ready_accounts.pop_account_txs(account_addr);
//             repush_txs.insert(repush_txs.end(), tx_ents.begin(), tx_ents.end());
//         }
//     }
// }

// for (auto &tx_ent : repush_txs) {
//     if (tx_ent->get_tx()->is_self_tx() || tx_ent->get_tx()->is_send_tx()) {
//         push_send_tx(tx_ent);
//     } else {
//         push_receipt(tx_ent);
//     }
// }
// }

void xtxpool_table_t::update_table_state(const data::xtablestate_ptr_t & table_state) {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    m_xtable_info.set_unconfirm_tx_count((int32_t)table_state->get_receiptid_state()->get_unconfirm_tx_num());
}

void xtxpool_table_t::add_role(xtxpool_role_info_t * role) {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    m_xtable_info.add_role(role);
}

void xtxpool_table_t::remove_role(xtxpool_role_info_t * role) {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    m_xtable_info.remove_role(role);
}

bool xtxpool_table_t::no_role() const {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    return m_xtable_info.no_role();
}

const std::vector<xtxpool_table_lacking_receipt_ids_t> xtxpool_table_t::get_lacking_recv_tx_ids(uint32_t & total_num) const {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    return m_txmgr_table.get_lacking_recv_tx_ids(m_xtable_info.get_all_table_sids(), total_num);
}

int32_t xtxpool_table_t::verify_cons_tx(const xcons_transaction_ptr_t & tx) const {
    int32_t ret;
    if (tx->is_send_tx() || tx->is_self_tx()) {
        ret = verify_send_tx(tx, false);  // backup check leader txs
    } else if (tx->is_recv_tx() || tx->is_confirm_tx()) {
        ret = verify_receipt_tx(tx);
    } else {
        ret = xtxpool_error_tx_invalid_type;
    }
    return ret;
}

int32_t xtxpool_table_t::verify_send_tx(const xcons_transaction_ptr_t & tx, bool is_first_time_push_tx) const {
    // 1. validation check
    int32_t ret = xverifier::xtx_verifier::verify_send_tx_validation(tx->get_transaction());
    if (ret) {
        return ret;
    }
    // 2. legal check, include hash/signature check and white/black check
    ret = xverifier::xtx_verifier::verify_send_tx_legitimacy(tx->get_transaction());
    if (ret) {
        return ret;
    }
    // 3. tx duration expire check
    uint64_t now = xverifier::xtx_utl::get_gmttime_s();
    ret = xverifier::xtx_verifier::verify_tx_fire_expiration(tx->get_transaction(), now, is_first_time_push_tx);
    if (ret) {
        return ret;
    }
    return xsuccess;
}

void xtxpool_table_t::filter_txs_by_black_white_list(std::vector<xcons_transaction_ptr_t> & txs) {
    auto whitelist_enable = xverifier::xwhitelist_utl::is_whitelist_enable();
    std::set<std::string> write_addrs;
    auto black_addrs = xverifier::xblacklist_utl_t::black_config();
    if (whitelist_enable) {
        write_addrs = xverifier::xwhitelist_utl::whitelist_config();
        if (write_addrs.empty()) {
            txs.clear();
            xwarn("xtxpool_table_t::filter_txs_by_black_white_list fail-whitelist empty,all txs filted.");
            return;
        }
    }
    if (write_addrs.empty() && black_addrs.empty()) {
        return;
    }

    auto iter = txs.begin();
    for (; iter != txs.end();) {
        if (!(*iter)->is_send_or_self_tx()) {
            iter++;
            continue;
        }
        auto & source_addr = (*iter)->get_source_addr();
        auto & target_addr = (*iter)->get_target_addr();
        if (!write_addrs.empty() && std::find(write_addrs.begin(), write_addrs.end(), source_addr) == std::end(write_addrs)) {
            xwarn("xtxpool_table_t::filter_txs_by_black_white_list fail-whitelist limit address,tx:%s", (*iter)->dump().c_str());
            iter = txs.erase(iter);
            continue;
        }
        if (std::find(black_addrs.begin(), black_addrs.end(), source_addr) != std::end(black_addrs) ||
            std::find(black_addrs.begin(), black_addrs.end(), target_addr) != std::end(black_addrs)) {
            xwarn("xtxpool_table_t::filter_txs_by_black_white_list fail-pop black addr tx,tx:%s", (*iter)->dump().c_str());
            tx_info_t info(*iter);
            pop_tx(info, true);
            iter = txs.erase(iter);
            continue;
        }
        iter++;
    }
}

int32_t xtxpool_table_t::verify_receipt_tx(const xcons_transaction_ptr_t & tx) const {
    XMETRICS_TIME_RECORD("txpool_message_unit_receipt_push_receipt_verify_receipt_tx");
    if (tx->is_confirm_tx()) {
        if (tx->get_last_not_need_confirm()) {
            xtxpool_info("xtxpool_table_t::verify_receipt_tx not need confirm.tx=%s", tx->dump(true).c_str());
            return xtxpool_error_not_need_confirm;
        }
    }

    if (!tx->verify_cons_transaction()) {
        return xtxpool_error_receipt_invalid;
    }

    std::string prove_account;
    xobject_ptr_t<base::xvqcert_t> prove_cert = tx->get_receipt_prove_cert_and_account(prove_account);
    if (nullptr == prove_cert) {
        xtxpool_error("xtxpool_table_t::verify_receipt_tx fail get prove cert, tx:%s", tx->dump(true).c_str());
        return xtxpool_error_tx_multi_sign_error;
    }

    XMETRICS_GAUGE(metrics::cpu_ca_verify_multi_sign_txreceipt, 1);
    base::enum_vcert_auth_result auth_result = m_para->get_certauth()->verify_muti_sign(prove_cert.get(), prove_account);
    if (auth_result != base::enum_vcert_auth_result::enum_successful) {
        int32_t ret = xtxpool_error_tx_multi_sign_error;
        xtxpool_warn("xtxpool_table_t::verify_receipt_tx fail. account=%s,tx=%s,auth_result:%d,fail-%u", prove_account.c_str(), tx->dump(true).c_str(), auth_result, ret);
        return ret;
    }
    return xsuccess;
}

bool xtxpool_table_t::get_account_latest_nonce(const std::string account_addr, uint64_t & latest_nonce) {
    refresh_table(true);

    common::xaccount_address_t  account_address(account_addr);  // TODO(jimmy)  common::xaccount_address_t include xvaccount_t for performance

    base::xaccount_index_t account_index;
    if (false == statestore::xstatestore_hub_t::instance()->get_accountindex(LatestConnectBlock, account_address, account_index)) {
        xtxpool_warn("xtxpool_table_t::get_account_latest_nonce fail-get account index.account:%s", account_addr.c_str());
        return false;        
    }
    latest_nonce = account_index.get_latest_tx_nonce();

    xtxpool_dbg("xtxpool_table_t::get_account_latest_nonce table:%s,height:%llu,account:%s,index:%s",
                m_xtable_info.get_account().c_str(),
                m_table_state_cache.get_state_height(),
                account_addr.c_str(),
                account_index.dump().c_str());
    return true;
}

const std::vector<xtxpool_table_lacking_receipt_ids_t> xtxpool_table_t::get_lacking_confirm_tx_ids(uint32_t & total_num) const {
    auto need_confirm_ids_vec = m_unconfirm_id_height.get_sender_all_need_confirm_ids(m_para->get_receiptid_state_cache(), true);
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    return m_txmgr_table.get_lacking_discrete_confirm_tx_ids(need_confirm_ids_vec, total_num);
}

xcons_transaction_ptr_t xtxpool_table_t::build_receipt(base::xtable_shortid_t peer_table_sid, uint64_t receipt_id, uint64_t commit_height, enum_transaction_subtype subtype) {
    base::xauto_ptr<base::xvblock_t> commit_block =
        m_para->get_vblockstore()->load_block_object(m_xtable_info, commit_height, base::enum_xvblock_flag_committed, false, metrics::blockstore_access_from_txpool_create_receipt);
    if (commit_block == nullptr) {
        xerror("xtxpool_table_t::build_receipt fail-commit table block not exist table=%s,peer table:%d,receipt id:%llu,table_height:%llu,subtype:%d",
               m_xtable_info.get_account().c_str(),
               peer_table_sid,
               receipt_id,
               commit_height,
               subtype);
        return nullptr;
    }

    base::xauto_ptr<base::xvblock_t> cert_block =
        m_para->get_vblockstore()->load_block_object(m_xtable_info, commit_height + 2, 0, false, metrics::blockstore_access_from_txpool_create_receipt);
    if (cert_block == nullptr) {
        xerror("xtxpool_table_t::build_receipt fail-cert table block not exist table=%s,peer table:%d,receipt id:%llu,table_height:%llu,subtype:%d",
               m_xtable_info.get_account().c_str(),
               peer_table_sid,
               receipt_id,
               commit_height + 2,
               subtype);
        return nullptr;
    }

    m_para->get_vblockstore()->load_block_input(commit_block->get_account(), commit_block.get());

    auto tx = xblocktool_t::create_one_txreceipt(dynamic_cast<xblock_t *>(commit_block.get()), dynamic_cast<xblock_t *>(cert_block.get()), peer_table_sid, receipt_id, subtype);
    xassert(tx != nullptr);
    return tx;
}

void xtxpool_table_t::build_recv_tx(base::xtable_shortid_t peer_table_sid, std::vector<uint64_t> receiptids, std::vector<xcons_transaction_ptr_t> & receipts) {
    auto self_table_sid = m_xtable_info.get_short_table_id();
    auto self_confirmid = m_para->get_receiptid_state_cache().get_confirmid_max(self_table_sid, peer_table_sid);
    auto peer_recvid = m_para->get_receiptid_state_cache().get_recvid_max(peer_table_sid, self_table_sid);
    uint64_t id_lower_bound = (self_confirmid > peer_recvid) ? self_confirmid : peer_recvid;
    for (auto & receiptid : receiptids) {
        if (receiptid <= id_lower_bound) {
            continue;
        }
        uint64_t commit_height;
        bool need_confirm = false;
        bool ret = m_unconfirm_id_height.get_sender_table_height_by_id(peer_table_sid, receiptid, commit_height, need_confirm);
        if (!ret) {
            xtxpool_info("xtxpool_table_t::build_recv_tx fail-receipt id not found self:%d,peer:%d,receipt id:%lu", self_table_sid, peer_table_sid, receiptid);
            continue;
        }

        auto receipt = build_receipt(peer_table_sid, receiptid, commit_height, enum_transaction_subtype_send);
        if (receipt != nullptr) {
            receipts.push_back(receipt);
        }
    }
}

void xtxpool_table_t::build_confirm_tx(base::xtable_shortid_t peer_table_sid, std::vector<uint64_t> receiptids, std::vector<xcons_transaction_ptr_t> & receipts) {
    auto self_table_sid = m_xtable_info.get_short_table_id();
    auto peer_confirmid = m_para->get_receiptid_state_cache().get_confirmid_max(peer_table_sid, self_table_sid);
    for (auto & receiptid : receiptids) {
        if (receiptid <= peer_confirmid) {
            continue;
        }
        uint64_t commit_height;
        bool need_confirm = false;
        bool ret = m_unconfirm_id_height.get_receiver_table_height_by_id(peer_table_sid, receiptid, commit_height, need_confirm);
        if (!ret) {
            xtxpool_info("xtxpool_table_t::build_confirm_tx fail-receipt id not found self:%d,peer:%d,receipt id:%lu", self_table_sid, peer_table_sid, receiptid);
            continue;
        }

        if (!need_confirm) {
            xtxpool_warn("xtxpool_table_t::build_confirm_tx receipt id not need confirm self:%d,peer:%d,receipt id:%lu", self_table_sid, peer_table_sid, receiptid);
            continue;
        }

        auto receipt = build_receipt(peer_table_sid, receiptid, commit_height, enum_transaction_subtype_recv);
        if (receipt != nullptr) {
            receipts.push_back(receipt);
        }
    }
}

void xtxpool_table_t::unconfirm_cache_status(uint32_t & sender_cache_size, uint32_t & receiver_cache_size, uint32_t & height_record_size, uint32_t & unconfirm_raw_txs_size) const {
    m_unconfirm_id_height.cache_status(sender_cache_size, receiver_cache_size, height_record_size);
    unconfirm_raw_txs_size = m_unconfirm_raw_txs.size();
    if (sender_cache_size > 1000 || receiver_cache_size > 1000 || height_record_size > 100 || unconfirm_raw_txs_size > 1000) {
        xwarn("xtxpool_table_t::unconfirm_cache_status table:%d cache:sender=%u,receiver=%u,height=%u,txs:%u",
              m_xtable_info.get_short_table_id(),
              sender_cache_size,
              receiver_cache_size,
              height_record_size,
              unconfirm_raw_txs_size);
    }
}

void xtxpool_table_t::get_min_keep_height(std::string & table_addr, uint64_t & height) const {
    bool need_sync = true;
    uint64_t min_unconfirm_height = 0;
    bool ret = m_unconfirm_id_height.get_min_height(m_para->get_receiptid_state_cache(), m_xtable_info.get_all_table_sids(), min_unconfirm_height, false, need_sync);
    table_addr = m_xtable_info.get_address();
    if (ret) {
        uint64_t more_height_num_to_keep = table_sync_on_demand_num_max * 2;
        height = (min_unconfirm_height > more_height_num_to_keep) ? (min_unconfirm_height - more_height_num_to_keep) : 0;
        xtxpool_info("xtxpool_table_t::get_min_keep_height table:%s,height:%llu", table_addr.c_str(), height);
    } else {
        height = 0;
        xtxpool_info("xtxpool_table_t::get_min_keep_height fail table:%s", m_xtable_info.get_address().c_str());
    }
}

void xtxpool_table_t::move_uncommit_txs(base::xvblock_t * block) {
    base::xvaccount_t _vaccount(block->get_account());
    if (false == base::xvchain_t::instance().get_xblockstore()->load_block_input(_vaccount, block, metrics::blockstore_access_from_txpool_refresh_table)) {
        xerror("xtxpool_table_t::move_uncommit_txs fail-load block input output, block=%s", block->dump().c_str());
        return;
    }

    std::map<std::string, xcons_transaction_ptr_t> send_txs;
    std::map<std::string, xcons_transaction_ptr_t> receipts;

    auto tx_actions = data::xblockextract_t::unpack_txactions(block);
    xdbg("xtxpool_table_t::move_uncommit_txs table block:%s", block->dump().c_str());

    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    for (auto & txaction : tx_actions) {
        auto & tx_hash = txaction.get_tx_hash();
        auto tx = m_txmgr_table.pop_tx(tx_hash, txaction.get_tx_subtype(), false);
        bool is_send_tx = (txaction.get_tx_subtype() == base::enum_transaction_subtype_send || txaction.get_tx_subtype() == base::enum_transaction_subtype_self);
        if (tx != nullptr) {
            if (is_send_tx) {
                send_txs[tx_hash] = tx;
            } else {
                receipts[tx_hash] = tx;
            }
        } else {
            if (is_send_tx) {
                xblock_t * table_block = dynamic_cast<xblock_t *>(block);
                xtransaction_ptr_t _rawtx = table_block->query_raw_transaction(txaction.get_tx_hash());
                if (_rawtx != nullptr) {
                    data::xcons_transaction_ptr_t cons_tx = make_object_ptr<data::xcons_transaction_t>(_rawtx.get());
                    send_txs[tx_hash] = cons_tx;
                } else {
                    xtxpool_error("xtxpool_table_t::move_uncommit_txs get raw tx fail table:%s,peer table:%d,tx type:%d,receipt_id::%llu,confirm id:%llu,table_height:%llu",
                                  m_xtable_info.get_account().c_str(),
                                  txaction.get_receipt_id_peer_tableid(),
                                  txaction.get_tx_subtype(),
                                  txaction.get_receipt_id(),
                                  txaction.get_sender_confirmed_receipt_id(),
                                  block->get_height());
                }
            } else {
                xtxpool_info("xtxpool_table_t::move_uncommit_txs receipt not found from cache.table:%s,peer table:%d,tx type:%d,receipt_id::%llu,confirm id:%llu,table_height:%llu",
                             m_xtable_info.get_account().c_str(),
                             txaction.get_receipt_id_peer_tableid(),
                             txaction.get_tx_subtype(),
                             txaction.get_receipt_id(),
                             txaction.get_sender_confirmed_receipt_id(),
                             block->get_height());
            }
        }
    }
    m_uncommit_txs.update_block_txs(block->get_height(), block->get_block_hash(), send_txs, receipts);
}

void xtxpool_table_t::update_uncommit_txs(base::xvblock_t * _lock_block, base::xvblock_t * _cert_block) {
    std::vector<xcons_transaction_ptr_t> recovered_send_txs;
    std::vector<xcons_transaction_ptr_t> recovered_receipts;

    auto ret = m_uncommit_txs.pop_recovered_block_txs(_cert_block->get_height(), _cert_block->get_block_hash(), _lock_block->get_block_hash(), recovered_send_txs, recovered_receipts);
    if (ret == no_need_update) {
        return;
    }

    // recovered txs push back to txmgr.
    for (auto & send_tx : recovered_send_txs) {
        xtxpool_v2::xtx_para_t para;
        std::shared_ptr<xtxpool_v2::xtx_entry> tx_ent = std::make_shared<xtxpool_v2::xtx_entry>(send_tx, para);
        push_send_tx_real(tx_ent);
    }
    for (auto & receipt : recovered_receipts) {
        xtxpool_v2::xtx_para_t para;
        std::shared_ptr<xtxpool_v2::xtx_entry> tx_ent = std::make_shared<xtxpool_v2::xtx_entry>(receipt, para);
        push_receipt_real(tx_ent);
    }

    if (ret == update_cert_and_lock) {
        move_uncommit_txs(_lock_block);
    }
    move_uncommit_txs(_cert_block);
}

xtransaction_ptr_t xtxpool_table_t::get_raw_tx(base::xtable_shortid_t peer_table_sid, uint64_t receipt_id) const {
    return m_unconfirm_raw_txs.get_raw_tx(peer_table_sid, receipt_id);
}

uint32_t xtxpool_table_t::get_tx_cache_size() const {
    std::lock_guard<std::mutex> lck(m_mgr_mutex);
    return m_txmgr_table.get_tx_cache_size();
}

}  // namespace xtxpool_v2
}  // namespace top
