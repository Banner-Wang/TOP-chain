﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include <cinttypes>
#include "xblockmaker/xblockmaker_error.h"
#include "xblockmaker/xtable_maker.h"
#include "xblockmaker/xtable_builder.h"
#include "xblockmaker/xblock_builder.h"
#include "xdata/xblocktool.h"
#include "xdata/xblockbuild.h"
#include "xdata/xethreceipt.h"
#include "xdata/xethbuild.h"
#include "xconfig/xpredefined_configurations.h"
#include "xconfig/xconfig_register.h"
#include "xchain_fork/xchain_upgrade_center.h"
#include "xgasfee/xgas_estimate.h"
#include "xtxexecutor/xbatchtx_executor.h"
#include "xstatectx/xstatectx.h"
#include "xverifier/xtx_verifier.h"

NS_BEG2(top, blockmaker)

// unconfirm rate limit parameters
#define table_pair_unconfirm_tx_num_max (512)
#define table_total_unconfirm_tx_num_max  (2048)
#define RELAY_BLOCK_TXS_PACK_NUM_MAX    (32)

xtable_maker_t::xtable_maker_t(const std::string & account, const xblockmaker_resources_ptr_t & resources)
: xblock_maker_t(account, resources) {
    xdbg("xtable_maker_t::xtable_maker_t create,this=%p,account=%s", this, account.c_str());
    m_fulltable_builder = std::make_shared<xfulltable_builder_t>();
    m_emptytable_builder = std::make_shared<xemptytable_builder_t>();
    m_default_builder_para = std::make_shared<xblock_builder_para_face_t>(resources);
    m_full_table_interval_num = XGET_CONFIG(fulltable_interval_block_num);
}

xtable_maker_t::~xtable_maker_t() {
    xdbg("xtable_maker_t::xtable_maker_t destroy,this=%p", this);
}

bool xtable_maker_t::can_make_next_full_block(const data::xblock_consensus_para_t & cs_para) const {
    uint64_t proposal_height = cs_para.get_latest_cert_block()->get_height() + 1;
    if ((proposal_height & (m_full_table_interval_num - 1)) == 0) {
        return true;
    }
    return false;
}

void xtable_maker_t::set_packtx_metrics(const xcons_transaction_ptr_t & tx, bool bsucc) const {
    #ifdef ENABLE_METRICS
    XMETRICS_GAUGE(metrics::cons_packtx_succ, bsucc ? 1 : 0);
    if (tx->is_self_tx() || tx->is_send_tx()) {
        XMETRICS_GAUGE(metrics::cons_packtx_sendtx_succ, bsucc ? 1 : 0);
    } else if (tx->is_recv_tx()) {
        XMETRICS_GAUGE(metrics::cons_packtx_recvtx_succ, bsucc ? 1 : 0);
    } else if (tx->is_confirm_tx()) {
        XMETRICS_GAUGE(metrics::cons_packtx_confirmtx_succ, bsucc ? 1 : 0);
    }
    #endif
}

xblock_ptr_t xtable_maker_t::make_light_table_v2(bool is_leader, const xtablemaker_para_t & table_para, const data::xblock_consensus_para_t & cs_para, xtablemaker_result_t & table_result) {
    const std::vector<xcons_transaction_ptr_t> & input_table_txs = table_para.get_origin_txs();
    if (!is_make_relay_chain() && input_table_txs.empty()) {
        table_result.m_make_block_error_code = xblockmaker_error_no_need_make_table;
        xwarn("xtable_maker_t::make_light_table_v2 fail-no input txs");
        return nullptr;
    }

    // create statectx
    statectx::xstatectx_para_t statectx_para(cs_para.get_clock());
    statectx::xstatectx_ptr_t statectx_ptr = statectx::xstatectx_factory_t::create_latest_cert_statectx(cs_para.get_latest_cert_block().get(), table_para.get_tablestate(), table_para.get_commit_tablestate(), statectx_para);
    int64_t tgas_balance_change = 0;
    std::vector<xblock_ptr_t> batch_units;
    std::vector<data::xlightunit_tx_info_ptr_t> txs_info;
    std::map<std::string, std::string> property_hashs;
    txexecutor::xexecute_output_t execute_output;

    if (!input_table_txs.empty()) {
        // TODO(jimmy) 
        uint64_t now = (uint64_t)base::xtime_utl::gettimeofday();

        // create batch executor
        uint64_t gas_limit = XGET_ONCHAIN_GOVERNANCE_PARAMETER(block_gas_limit);
        txexecutor::xvm_para_t vmpara(cs_para.get_clock(), cs_para.get_random_seed(), cs_para.get_total_lock_tgas_token(), gas_limit, cs_para.get_table_proposal_height(), cs_para.get_coinbase());
        txexecutor::xbatchtx_executor_t executor(statectx_ptr, vmpara);

        std::vector<xcons_transaction_ptr_t> input_txs;
        for (auto & tx : input_table_txs) {
            if (tx->get_transaction() == nullptr) {
                if (tx->is_confirm_tx()) {
                    auto raw_tx = get_txpool()->get_raw_tx(tx->get_account_addr(), tx->get_peer_tableid(), tx->get_last_action_receipt_id());
                    if (raw_tx == nullptr) {
                        XMETRICS_GAUGE(metrics::cons_packtx_fail_load_origintx, 1);
                        xwarn("xtable_maker_t::make_light_table_v2 fail-tx filtered load origin tx.%s tx=%s", cs_para.dump().c_str(), tx->dump().c_str());
                        continue;
                    }
                    if (false == tx->set_raw_tx(raw_tx.get())) {
                        xerror("xtable_maker_t::make_light_table_v2 fail-tx filtered set origin tx.%s tx=%s", cs_para.dump().c_str(), tx->dump().c_str());
                        continue;
                    }
                }
            }

            // TODO(jimmy) leader add sendtx expire check, should do this in txpool future
            if (is_leader) {
                if (tx->is_send_or_self_tx()) {
                    if (xsuccess != xverifier::xtx_verifier::verify_tx_fire_expiration(tx->get_transaction(), now, false)) {
                        xtxpool_v2::tx_info_t txinfo(tx->get_source_addr(), tx->get_tx_hash_256(), tx->get_tx_subtype());
                        get_txpool()->pop_tx(txinfo);
                        xwarn("xtable_maker_t::make_light_table_v2 fail-tx filtered expired.is_leader=%d,%s tx=%s", is_leader, cs_para.dump().c_str(), tx->dump().c_str());
                        continue;
                    }
                }
            }

            // update tx flag before execute
            input_txs.push_back(tx);
        }
        if (input_txs.empty()
            || (!is_leader && input_txs.size() != input_table_txs.size())) {
            table_result.m_make_block_error_code = xblockmaker_error_no_need_make_table;
            xwarn("xtable_maker_t::make_light_table_v2 fail tx is filtered.is_leader=%d,%s,txs_size=%zu:%zu", is_leader, cs_para.dump().c_str(), input_txs.size(), input_table_txs.size());
            return nullptr;
        }

        // execute all txs
        int32_t ret = executor.execute(input_txs, execute_output);
        if (ret != xsuccess || execute_output.empty()) {
            table_result.m_make_block_error_code = xblockmaker_error_no_need_make_table;
            xerror("xtable_maker_t::make_light_table_v2 fail execute all txs.is_leader=%d,%s,txs_size=%zu", is_leader, cs_para.dump().c_str(), input_txs.size());
            return nullptr;
        }

        // save txs need packed
        xunitbuildber_txkeys_mgr_t  txkeys_mgr;
        for (auto & txout : execute_output.pack_outputs) {
            xinfo("xtable_maker_t::make_light_table_v2 packtx is_leader=%d,%s,tx=%s,txout=%s,action=%s", 
                is_leader, cs_para.dump().c_str(), txout.m_tx->dump().c_str(), txout.dump().c_str(),txout.m_tx->dump_execute_state().c_str());
            table_para.push_tx_to_proposal(txout.m_tx);  // set pack origin tx to proposal
            txs_info.push_back(build_tx_info(txout.m_tx));
            txkeys_mgr.add_pack_tx(txout.m_tx);
            tgas_balance_change += txout.m_vm_output.m_tgas_balance_change;
            for (auto & v : txout.m_vm_output.m_contract_create_txs) {
                txs_info.push_back(build_tx_info(v));
                txkeys_mgr.add_pack_tx(v);
            }
        }

        for (auto & txout : execute_output.drop_outputs) {
            xinfo("xtable_maker_t::make_light_table_v2 droptx is_leader=%d,%s,tx=%s,txout=%s,action=%s", 
                is_leader, cs_para.dump().c_str(), txout.m_tx->dump().c_str(), txout.dump().c_str(),txout.m_tx->dump_execute_state().c_str());        
            xtxpool_v2::tx_info_t txinfo(txout.m_tx->get_source_addr(), txout.m_tx->get_tx_hash_256(), txout.m_tx->get_tx_subtype());
            get_txpool()->pop_tx(txinfo);
        }

        for (auto & txout : execute_output.nopack_outputs) {
            xinfo("xtable_maker_t::make_light_table_v2 nopacktx is_leader=%d,%s,tx=%s,txout=%s,action=%s", 
                is_leader, cs_para.dump().c_str(), txout.m_tx->dump().c_str(), txout.dump().c_str(),txout.m_tx->dump_execute_state().c_str());        
        }

        if (txs_info.empty()) {
            table_result.m_make_block_error_code = xblockmaker_error_no_need_make_table;
            xwarn("xtable_maker_t::make_light_table_v2 fail-no pack txs.is_leader=%d,%s,txs_size=%zu", is_leader, cs_para.dump().c_str(), input_txs.size());
            return nullptr;
        }

        // create units
        std::vector<statectx::xunitstate_ctx_ptr_t> unitctxs = statectx_ptr->get_modified_unit_ctx();
        if (unitctxs.empty()) {
            xwarn("xtable_maker_t::make_light_table_v2 fail-no unitstates changed.is_leader=%d,%s,txs_size=%zu", is_leader, cs_para.dump().c_str(), input_txs.size());
        }
        for (auto & unitctx : unitctxs) {
            base::xvtxkey_vec_t txkeys = txkeys_mgr.get_account_txkeys(unitctx->get_unitstate()->get_address());
            if (txkeys.get_txkeys().empty()) {
                // will support state change without txkeys for evm tx
                xwarn("xtable_maker_t::make_light_table_v2 fail-txkeys empty.is_leader=%d,%s,addr=%s,txs_size=%zu", is_leader, cs_para.dump().c_str(), unitctx->get_unitstate()->get_address().c_str(), input_txs.size());
                // return nullptr;
            }
            xunitbuilder_para_t unit_para(txkeys);
            data::xblock_ptr_t unitblock = xunitbuilder_t::make_block(unitctx->get_prev_block(), unitctx->get_unitstate(), unit_para, cs_para);
            if (nullptr == unitblock) {
                // should not fail
                xerror("xtable_maker_t::make_light_table_v2 fail-invalid unitstate.is_leader=%d,%s,txs_size=%zu", is_leader, cs_para.dump().c_str(), input_txs.size());
                return nullptr;
            }
            batch_units.push_back(unitblock);
            xinfo("xtable_maker_t::make_light_table_v2 succ-make unit.is_leader=%d,unit=%s,txkeys=%zu",is_leader, unitblock->dump().c_str(),txkeys.get_txkeys().size());
            xtablebuilder_t::update_account_index_property(statectx_ptr->get_table_state(), unitblock, unitctx->get_unitstate());
        }

        auto self_table_sid = get_short_table_id();
        auto & receiptid_info_map = table_para.get_receiptid_info_map();
        std::map<base::xtable_shortid_t, uint64_t> changed_confirm_ids;
        for (auto & receiptid_info : receiptid_info_map) {
            auto & peer_table_sid = receiptid_info.first;

            base::xreceiptid_pair_t peer_pair;
            receiptid_info.second.m_receiptid_state->find_pair(self_table_sid, peer_pair);
            auto recvid_max = peer_pair.get_recvid_max();
            changed_confirm_ids[peer_table_sid] = recvid_max;
        }

        xtablebuilder_t::update_receipt_confirmids(statectx_ptr->get_table_state(), changed_confirm_ids);
        xtablebuilder_t::make_table_prove_property_hashs(statectx_ptr->get_table_state()->get_bstate().get(), property_hashs);
    }

    if (!set_relay_para(cs_para, table_para, is_leader)) {
        table_result.m_make_block_error_code = xblockmaker_error_proposal_bad_header;
        return nullptr;
    }

    // reset justify cert hash para
    const xblock_ptr_t & cert_block = cs_para.get_latest_cert_block();
    const xblock_ptr_t & lock_block = cs_para.get_latest_locked_block();
    if (lock_block == nullptr) {
        xerror("xtable_maker_t::make_light_table_v2 fail-get block block.is_leader=%d,%s", is_leader, cs_para.dump().c_str());
        return nullptr;
    }
    cs_para.set_justify_cert_hash(lock_block->get_input_root_hash());
    cs_para.set_parent_height(0);

    cs_para.set_ethheader(xeth_header_builder::build(cs_para, execute_output.pack_outputs));

    data::xblock_ptr_t tableblock = xtablebuilder_t::make_light_block(cs_para.get_latest_cert_block(),
                                                                        statectx_ptr->get_table_state(),
                                                                        cs_para,
                                                                        tgas_balance_change,
                                                                        batch_units,
                                                                        txs_info,
                                                                        property_hashs);
    if (nullptr != tableblock) {
        xinfo("xtable_maker_t::make_light_table_v2-succ is_leader=%d,%s,packtxs=%zu,units=%zu,gaspledge=%ld", 
            is_leader, cs_para.dump().c_str(), txs_info.size(), batch_units.size(), tgas_balance_change);
    }
    return tableblock;
}

xblock_ptr_t xtable_maker_t::leader_make_light_table(const xtablemaker_para_t & table_para, const data::xblock_consensus_para_t & cs_para, xtablemaker_result_t & table_result) {
    XMETRICS_TIMER(metrics::cons_make_lighttable_tick);
    return make_light_table_v2(true, table_para, cs_para, table_result);
}

xblock_ptr_t xtable_maker_t::backup_make_light_table(const xtablemaker_para_t & table_para, const data::xblock_consensus_para_t & cs_para, xtablemaker_result_t & table_result) {
    XMETRICS_TIMER(metrics::cons_verify_lighttable_tick);
    return make_light_table_v2(false, table_para, cs_para, table_result);
}

xblock_ptr_t xtable_maker_t::make_full_table(const xtablemaker_para_t & table_para, const xblock_consensus_para_t & cs_para, int32_t & error_code) {
    XMETRICS_TIMER(metrics::cons_make_fulltable_tick);
    std::vector<xblock_ptr_t> blocks_from_last_full;
    if (false == load_table_blocks_from_last_full(cs_para.get_latest_cert_block(), blocks_from_last_full)) {
        xerror("xtable_maker_t::make_full_table fail-load blocks. %s", cs_para.dump().c_str());
        return nullptr;
    }

    // reset justify cert hash para
    const xblock_ptr_t & cert_block = cs_para.get_latest_cert_block();
    const xblock_ptr_t & lock_block = cs_para.get_latest_locked_block();
    if (lock_block == nullptr) {
        xerror("xtable_maker_t::make_full_table fail-get block block.%s", cs_para.dump().c_str());
        return nullptr;
    }
    cs_para.set_justify_cert_hash(lock_block->get_input_root_hash());
    cs_para.set_parent_height(0);
    data::xtablestate_ptr_t tablestate = table_para.get_tablestate();
    xassert(nullptr != tablestate);

    cs_para.set_ethheader(xeth_header_builder::build(cs_para));

    xblock_builder_para_ptr_t build_para = std::make_shared<xfulltable_builder_para_t>(tablestate, blocks_from_last_full, get_resources());
    xblock_ptr_t proposal_block = m_fulltable_builder->build_block(cert_block, table_para.get_tablestate()->get_bstate(), cs_para, build_para);
    if (proposal_block == nullptr) {
        xwarn("xtable_maker_t::make_full_table fail-build block.%s", cs_para.dump().c_str());
        return nullptr;
    }
    xinfo("xtable_maker_t::make_full_table succ.block=%s", proposal_block->dump().c_str());
    return proposal_block;
}

xblock_ptr_t xtable_maker_t::make_empty_table(const xtablemaker_para_t & table_para, const xblock_consensus_para_t & cs_para, int32_t & error_code) {
    // TODO(jimmy)
    XMETRICS_TIME_RECORD("cons_tableblock_verfiy_proposal_imp_make_empty_table");

    // reset justify cert hash para
    const xblock_ptr_t & cert_block = cs_para.get_latest_cert_block();
    const xblock_ptr_t & lock_block = cs_para.get_latest_locked_block();
    if (lock_block == nullptr) {
        xerror("xtable_maker_t::make_empty_table fail-get block block.%s", cs_para.dump().c_str());
        return nullptr;
    }
    cs_para.set_justify_cert_hash(lock_block->get_input_root_hash());
    cs_para.set_parent_height(0);
    data::xtablestate_ptr_t tablestate = table_para.get_tablestate();
    xassert(nullptr != tablestate);

    cs_para.set_ethheader(xeth_header_builder::build(cs_para));

    xblock_ptr_t proposal_block = m_emptytable_builder->build_block(cert_block, table_para.get_tablestate()->get_bstate(), cs_para, m_default_builder_para);
    return proposal_block;
}

bool    xtable_maker_t::load_table_blocks_from_last_full(const xblock_ptr_t & prev_block, std::vector<xblock_ptr_t> & blocks) {
    std::vector<xblock_ptr_t> _form_highest_blocks;
    xblock_ptr_t current_block = prev_block;
    xassert(current_block->get_height() > 0);
    _form_highest_blocks.push_back(current_block);

    while (current_block->get_block_class() != base::enum_xvblock_class_full && current_block->get_height() > 1) {
        // only mini-block is enough
        base::xauto_ptr<base::xvblock_t> _block = get_blockstore()->load_block_object(*this, current_block->get_height() - 1, current_block->get_last_block_hash(), false, metrics::blockstore_access_from_blk_mk_table);
        if (_block == nullptr) {
            xerror("xfulltable_builder_t::load_table_blocks_from_last_full fail-load block.account=%s,height=%ld", get_account().c_str(), current_block->get_height() - 1);
            return false;
        }
        current_block = xblock_t::raw_vblock_to_object_ptr(_block.get());
        blocks.push_back(current_block);
    }

    // TOOD(jimmy) use map
    for (auto iter = _form_highest_blocks.rbegin(); iter != _form_highest_blocks.rend(); iter++) {
        blocks.push_back(*iter);
    }
    return true;;
}

xblock_ptr_t xtable_maker_t::make_proposal(xtablemaker_para_t & table_para,
                                           const data::xblock_consensus_para_t & cs_para,
                                           xtablemaker_result_t & tablemaker_result) {
    XMETRICS_TIMER(metrics::cons_tablemaker_make_proposal_tick);
    std::lock_guard<std::mutex> l(m_lock);
    // check table maker state
    const xblock_ptr_t & latest_cert_block = cs_para.get_latest_cert_block();
    xassert(table_para.get_tablestate() != nullptr);

    bool can_make_empty_table_block = can_make_next_empty_block(cs_para);
    if (table_para.get_origin_txs().empty() && !can_make_empty_table_block && !is_make_relay_chain()) {
        xinfo("xtable_maker_t::make_proposal no txs and no need make empty block. %s,cert_height=%" PRIu64 "", cs_para.dump().c_str(), latest_cert_block->get_height());
        return nullptr;
    }

    xblock_ptr_t proposal_block = nullptr;
    if (can_make_next_full_block(cs_para)) {
        proposal_block = make_full_table(table_para, cs_para, tablemaker_result.m_make_block_error_code);
    } else {
        proposal_block = leader_make_light_table(table_para, cs_para, tablemaker_result);
    }

    if (proposal_block == nullptr) {
        if (tablemaker_result.m_make_block_error_code != xblockmaker_error_no_need_make_table) {
            xwarn("xtable_maker_t::make_proposal fail-make table. %s,error_code=%s",
                cs_para.dump().c_str(), chainbase::xmodule_error_to_str(tablemaker_result.m_make_block_error_code).c_str());
            return nullptr;
        } else {
            if (can_make_empty_table_block) {
                proposal_block = make_empty_table(table_para, cs_para, tablemaker_result.m_make_block_error_code);
                if (proposal_block == nullptr) {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }
    }

    xinfo("xtable_maker_t::make_proposal succ.%s,proposal=%s,tx_count=%zu,other_accounts_count=%zu",
        cs_para.dump().c_str(), proposal_block->dump().c_str(), table_para.get_proposal()->get_input_txs().size(), table_para.get_proposal()->get_other_accounts().size());
    return proposal_block;
}

int32_t xtable_maker_t::verify_proposal(base::xvblock_t* proposal_block, const xtablemaker_para_t & table_para, const data::xblock_consensus_para_t & cs_para) {
    XMETRICS_TIMER(metrics::cons_tablemaker_verify_proposal_tick);
    std::lock_guard<std::mutex> l(m_lock);

    // check table maker state
    const xblock_ptr_t & latest_cert_block = cs_para.get_latest_cert_block();  // should update to this new table cert block
    xassert(table_para.get_tablestate() != nullptr);

    const auto & highest_block = latest_cert_block;
    if (proposal_block->get_last_block_hash() != highest_block->get_block_hash()
        || proposal_block->get_height() != highest_block->get_height() + 1) {
        xwarn("xtable_maker_t::verify_proposal fail-proposal unmatch last hash.proposal=%s, last_height=%" PRIu64 "",
            proposal_block->dump().c_str(), highest_block->get_height());
        return xblockmaker_error_proposal_table_not_match_prev_block;
    }

    const xblock_ptr_t & lock_block = cs_para.get_latest_locked_block();
    if (lock_block == nullptr) {
        xerror("xtable_maker_t::verify_proposal fail-get lock block.proposal=%s, last_height=%" PRIu64 "",
            proposal_block->dump().c_str(), highest_block->get_height());
        return xblockmaker_error_proposal_table_not_match_prev_block;
    }
    if (proposal_block->get_cert()->get_justify_cert_hash() != lock_block->get_input_root_hash()) {
        xerror("xtable_maker_t::verify_proposal fail-proposal unmatch justify hash.proposal=%s, last_height=%" PRIu64 ",justify=%s,%s",
            proposal_block->dump().c_str(), highest_block->get_height(),
            base::xstring_utl::to_hex(proposal_block->get_cert()->get_justify_cert_hash()).c_str(),
            base::xstring_utl::to_hex(lock_block->get_input_root_hash()).c_str());
        return xblockmaker_error_proposal_table_not_match_prev_block;
    }

    xblock_ptr_t local_block = nullptr;
    xtablemaker_result_t table_result;
    if (can_make_next_full_block(cs_para)) {
        local_block = make_full_table(table_para, cs_para, table_result.m_make_block_error_code);
    } else if (proposal_block->get_block_class() == base::enum_xvblock_class_nil) {
        if (can_make_next_empty_block(cs_para)) {
            local_block = make_empty_table(table_para, cs_para, table_result.m_make_block_error_code);
        }
    } else {
        local_block = backup_make_light_table(table_para, cs_para, table_result);
    }
    if (local_block == nullptr) {
        xwarn("xtable_maker_t::verify_proposal fail-make table. proposal=%s,error_code=%s",
            proposal_block->dump().c_str(), chainbase::xmodule_error_to_str(table_result.m_make_block_error_code).c_str());
        return table_result.m_make_block_error_code;
    }

    local_block->get_cert()->set_nonce(proposal_block->get_cert()->get_nonce());  // TODO(jimmy)
    // check local and proposal is match
    if (false == verify_proposal_with_local(proposal_block, local_block.get())) {
        xwarn("xtable_maker_t::verify_proposal fail-verify_proposal_with_local. proposal=%s",
            proposal_block->dump().c_str());
        XMETRICS_GAUGE(metrics::cons_fail_verify_proposal_table_with_local, 1);
        return xblockmaker_error_proposal_not_match_local;
    }

    xinfo("xtable_maker_t::verify_proposal succ.proposal=%s",
        proposal_block->dump().c_str());
    return xsuccess;
}

bool xtable_maker_t::verify_proposal_with_local(base::xvblock_t *proposal_block, base::xvblock_t *local_block) const {
    const std::vector<base::xventity_t*> & _proposal_table_inentitys = proposal_block->get_input()->get_entitys();
    const std::vector<base::xventity_t*> & _local_table_inentitys = local_block->get_input()->get_entitys();
    if (_proposal_table_inentitys.size() != _local_table_inentitys.size()) {
        xwarn("xtable_maker_t::verify_proposal_with_local fail-entity size not same. %s %s",
            proposal_block->dump().c_str(),
            local_block->dump().c_str());
        return false;
    }
    size_t entity_count = _proposal_table_inentitys.size();
    for (size_t i = 0; i < entity_count; i++) {
        base::xvinentity_t* _proposal_inentity = dynamic_cast<base::xvinentity_t*>(_proposal_table_inentitys[i]);
        base::xvinentity_t* _local_inentity = dynamic_cast<base::xvinentity_t*>(_local_table_inentitys[i]);
        if (_proposal_inentity == nullptr || _local_inentity == nullptr) {
            xerror("xtable_maker_t::verify_proposal_with_local fail-get inentity. %s", proposal_block->dump().c_str());  // should never happen
            return false;
        }
        if (i == 0) {  // entity#0 used for table self
            if (_proposal_inentity->get_extend_data() != _local_inentity->get_extend_data()) {
                xerror("xtable_maker_t::verify_proposal_with_local fail-get inentity. %s", proposal_block->dump().c_str());  // should never happen
                return false;
            }
            continue;
        }

        base::xtable_inentity_extend_t _proposal_extend;
        _proposal_extend.serialize_from_string(_proposal_inentity->get_extend_data());
        const xobject_ptr_t<base::xvheader_t> & _proposal_unit_header = _proposal_extend.get_unit_header();
        base::xtable_inentity_extend_t _local_extend;
        _local_extend.serialize_from_string(_local_inentity->get_extend_data());
        const xobject_ptr_t<base::xvheader_t> & _local_unit_header = _local_extend.get_unit_header();
        if (_proposal_unit_header->get_account() != _local_unit_header->get_account()
            || _proposal_unit_header->get_height() != _local_unit_header->get_height()
            || _proposal_unit_header->get_block_class() != _local_unit_header->get_block_class()) {
            xerror("xtable_maker_t::verify_proposal_with_local fail-unit entity not match. %s,leader=%s,local=%s",
                proposal_block->dump().c_str(),
                data::xblock_t::dump_header(_proposal_unit_header.get()).c_str(), data::xblock_t::dump_header(_local_unit_header.get()).c_str());
            return false;
        }
        if (_proposal_inentity->get_extend_data() != _local_inentity->get_extend_data()
         || _proposal_inentity->get_actions().size() != _local_inentity->get_actions().size()) {
            xwarn("xtable_maker_t::verify_proposal_with_local fail-extend data not match. %s,leader=%s,local=%s,action_size=%zu,%zu",
                proposal_block->dump().c_str(),
                data::xblock_t::dump_header(_proposal_unit_header.get()).c_str(), data::xblock_t::dump_header(_local_unit_header.get()).c_str(),
                _proposal_inentity->get_actions().size(), _local_inentity->get_actions().size());
            return false;
        }
    }

    if (local_block->get_input_hash() != proposal_block->get_input_hash()) {
        xwarn("xtable_maker_t::verify_proposal_with_local fail-input hash not match. %s %s",
            proposal_block->dump().c_str(),
            local_block->dump().c_str());
        return false;
    }
    if (local_block->get_output_hash() != proposal_block->get_output_hash()) {
        xwarn("xtable_maker_t::verify_proposal_with_local fail-output hash not match. %s %s",
            proposal_block->dump().c_str(),
            local_block->dump().c_str());
        return false;
    }
    if (local_block->get_header_hash() != proposal_block->get_header_hash()) {
        xwarn("xtable_maker_t::verify_proposal_with_local fail-header hash not match. %s proposal:%s local:%s",
            proposal_block->dump().c_str(),
            ((data::xblock_t*)proposal_block)->dump_header().c_str(),
            ((data::xblock_t*)local_block)->dump_header().c_str());
        return false;
    }
    if(!local_block->get_cert()->is_equal(*proposal_block->get_cert())){
        xerror("xtable_maker_t::verify_proposal_with_local fail-cert hash not match. proposal:%s local:%s",
            ((data::xblock_t*)proposal_block)->dump_cert().c_str(),
            ((data::xblock_t*)local_block)->dump_cert().c_str());
        return false;
    }
    bool bret = proposal_block->set_output_resources(local_block->get_output()->get_resources_data());
    if (!bret) {
        xerror("xtable_maker_t::verify_proposal_with_local fail-set proposal block output fail");
        return false;
    }
    bret = proposal_block->set_input_resources(local_block->get_input()->get_resources_data());
    if (!bret) {
        xerror("xtable_maker_t::verify_proposal_with_local fail-set proposal block input fail");
        return false;
    }
    return true;
}

bool xtable_maker_t::can_make_next_empty_block(const data::xblock_consensus_para_t & cs_para) const {
    if (is_make_relay_chain()) {
        return false;
    }
    const xblock_ptr_t & current_block = cs_para.get_latest_cert_block();
    if (current_block->get_height() == 0) {
        return false;
    }
    if (current_block->get_block_class() == base::enum_xvblock_class_light) {
        return true;
    }
    const xblock_ptr_t & lock_block = cs_para.get_latest_locked_block();
    if (lock_block == nullptr) {
        return false;
    }
    if (lock_block->get_block_class() == base::enum_xvblock_class_light) {
        return true;
    }
    return false;
}

bool xtable_maker_t::is_make_relay_chain() const {
    return get_zone_index() == base::enum_chain_zone_relay_index;
}

bool xtable_maker_t::get_last_relay_info(const data::xblock_consensus_para_t & cs_para,
                                         uint8_t & wrap_phase,
                                         relay_wrap_info_t & last_wrap_info,
                                         std::string & last_relay_block_data,
                                         data::xrelay_block & last_relay_block) {
    auto latest_wrap_block = cs_para.get_latest_cert_block();
    // wrap phase, evm table height, elect height store into wrap block header comments.
    data::xtableheader_extra_t last_header_extra;
    if (latest_wrap_block->is_genesis_block()) {
        wrap_phase = 2;
        last_wrap_info = relay_wrap_info_t(0, 0, 0);
        last_relay_block = data::xrootblock_t::get_genesis_relay_block();
    } else {
        if (latest_wrap_block->get_block_class() == base::enum_xvblock_class_full) {
            latest_wrap_block = cs_para.get_latest_locked_block();
        }

        std::error_code ec;
        data::xrelay_wrap_info_t wrap_info;
        data::xblockextract_t::unpack_relaywrapinfo_and_relay_block_data(latest_wrap_block.get(), wrap_info, last_relay_block_data, ec);
        if (ec) {
            xerror("xtable_maker_t::get_last_relay_info unpack relay info fail,proposal:%s", latest_wrap_block->dump().c_str());
            return false;
        }

        last_relay_block.decodeBytes(to_bytes(last_relay_block_data), ec);
        if (ec) {
            xerror("xtable_maker_t::get_last_relay_info decodeBytes error %s; err msg %s", ec.category().name(), ec.message().c_str());
            return false;
        }
        wrap_phase = wrap_info.get_wrap_phase();
        // todo(nathan): timestamp.
        last_wrap_info = relay_wrap_info_t(wrap_info.get_evm_height(), wrap_info.get_elect_height(), 0);
        xdbg("xtable_maker_t::get_last_relay_info wrap_phase:%u,last_wrap_info:evm h:%llu,elect h:%llu,timestamp:%llu",
             wrap_phase,
             last_wrap_info.evm_height(),
             last_wrap_info.elect_height(),
             last_wrap_info.poly_timestamp());
    }
    return true;
}

bool xtable_maker_t::build_relay_block_data_from_last_block(const data::xblock_consensus_para_t & cs_para, const std::string & last_relay_block_data, data::xrelay_block & last_relay_block, std::string & relay_block_data) {
    if (cs_para.get_election_round() < last_relay_block.get_inner_header().get_epochID()) {
        xwarn("xtable_maker_t::build_relay_block_data_from_last_block proposal epoch id fallbehind.relay_block:%s,proposal elect round:%lu", last_relay_block.dump().c_str(), cs_para.get_election_round());
        return false;
    }

    if (last_relay_block.get_inner_header().get_epochID() != cs_para.get_election_round()) {
        if (last_relay_block.get_inner_header().get_epochID() + 1 != cs_para.get_election_round()) {
            xwarn("xtable_maker_t::build_relay_block_data_from_last_block epochid exception.relay_block:%s,new round:%lu", last_relay_block.dump().c_str(), cs_para.get_election_round());
            return false;
        }
        xwarn("xtable_maker_t::build_relay_block_data_from_last_block epochid changed, should consensus from phase 0 again.relay_block:%s,new round:%lu",
                last_relay_block.dump().c_str(),
                cs_para.get_election_round());
        last_relay_block.get_header().set_epochid(cs_para.get_election_round());
        last_relay_block.build_finish();
        xbytes_t rlp_stream = last_relay_block.encodeBytes();
        relay_block_data = from_bytes<std::string>((xbytes_t)(rlp_stream));
    } else {
        relay_block_data = last_relay_block_data;
    }
    return true;
}

bool xtable_maker_t::set_relay_para(const data::xblock_consensus_para_t & cs_para, const xtablemaker_para_t & table_para, bool is_leader) {
    if (!is_make_relay_chain()) {
        return true;
    }

    auto & latest_cert_block = cs_para.get_latest_cert_block();
    // wrap phase, evm table height, elect height store into wrap block header comments.        
    uint8_t last_wrap_phase;
    relay_wrap_info_t last_wrap_info;
    data::xrelay_block last_relay_block;
    std::string last_relay_block_data;

    if (!get_last_relay_info(cs_para, last_wrap_phase, last_wrap_info, last_relay_block_data, last_relay_block)) {
        xerror("xtable_maker_t::set_relay_para get_last_relay_info fail.");
        return false;
    }

    base::xstream_t _stream(base::xcontext_t::instance());
    // wrap phase: 0, 1, 2
    uint8_t wrap_phase = last_wrap_phase + 1;
    relay_wrap_info_t new_wrap_info;
    data::xrelay_block relay_block;
    std::string relay_block_data;
    if (wrap_phase > 2) {
        wrap_phase = 0; 
        bool ret = true;
        if (is_leader) {
            ret = build_relay_block_data_leader(cs_para, last_relay_block, last_wrap_info, new_wrap_info, relay_block);
        } else {
            // todo(nathan):timestamp
            new_wrap_info = relay_wrap_info_t(table_para.get_relay_evm_height(), table_para.get_relay_elect_height(), 0);
            ret = build_relay_block_data_backup(cs_para, last_relay_block, last_wrap_info, new_wrap_info, relay_block);
        }
        if (!ret) {
            xinfo("xtable_maker_t::set_relay_para make relay block fail.cs_para:%s", cs_para.dump().c_str());
            return false;
        }
        xbytes_t rlp_stream = relay_block.encodeBytes();
        relay_block_data = from_bytes<std::string>((xbytes_t)(rlp_stream));
    } else {
        // todo(nathan):timestamp
        new_wrap_info = relay_wrap_info_t(last_wrap_info.evm_height(), last_wrap_info.elect_height(), 0);
        if (!build_relay_block_data_from_last_block(cs_para, last_relay_block_data, last_relay_block, relay_block_data)) {
            return false;
        }
        // todo(nathan): wrap phase should reset to 0.
        // if (last_relay_block.get_inner_header().get_epochID() + 1 != cs_para.get_election_round()) {
        //     wrap_phase = 0;
        // }

        if (wrap_phase == 2) {
            uint256_t hash256 = from_bytes<uint256_t>(last_relay_block.get_block_hash().to_bytes());
            cs_para.set_vote_extend_hash(hash256);
        }
    }
    xinfo("xtable_maker_t::set_relay_para make relay block succ.is_leader:%d,cs_para:%s,relay_block:%s,phase:%d,evm_h:%llu,ele_h:%llu",
            is_leader,
            cs_para.dump().c_str(),
            last_relay_block.dump().c_str(),
            wrap_phase,
            new_wrap_info.evm_height(),
            new_wrap_info.elect_height());

    // todo(nathan):timestamp
    std::string wrap_data = data::xrelay_wrap_info_t::build_relay_wrap_info_string(wrap_phase, new_wrap_info.evm_height(), new_wrap_info.elect_height());
    cs_para.set_relay_wrap_data(wrap_data);
    cs_para.set_relay_block_data(relay_block_data);
    cs_para.set_need_relay_prove(wrap_phase == 2);

    return true;
}

bool xtable_maker_t::build_relay_block_data_leader(const data::xblock_consensus_para_t & cs_para,
                                                   const data::xrelay_block & last_relay_block,
                                                   const relay_wrap_info_t & last_wrap_info,
                                                   relay_wrap_info_t & new_wrap_info,
                                                   data::xrelay_block & relay_block) {
    std::vector<data::xrelay_election_node_t> reley_election;
    auto ret = get_relay_chain_mgr()->get_elect_cache(last_wrap_info.elect_height() + 1, reley_election);
    if (ret) {
        // new election data.build elect relay block
        new_wrap_info = relay_wrap_info_t(last_wrap_info.evm_height(), last_wrap_info.elect_height() + 1, last_wrap_info.poly_timestamp());
        data::xrelay_election_group_t reley_election_group;
        reley_election_group.election_epochID = new_wrap_info.elect_height();
        reley_election_group.elections_vector = reley_election;
        relay_block = data::xrelay_block(
            last_relay_block.get_block_hash(), last_relay_block.get_block_height() + 1, cs_para.get_election_round(), cs_para.get_timestamp(), reley_election_group);
    } else {
        // no new election data. try build poly relay block or tx relay block.
        // todo(nathan):make poly relay block.
        // if((cs_para.get_timestamp() > last_wrap_info.poly_timestamp() + XGET_ONCHAIN_GOVERNANCE_PARAMETER(max_relay_poly_interval) * 10) && last_relay_block.get_all_receipts().size() > 0) {

        //     xinfo("xrelay_proposal_maker_t::build_relay_block_data_leader time last_poly_timestamp(%ld), timestamp(%ld)",
        //         last_wrap_info.poly_timestamp(), cs_para.get_timestamp());
        //     new_wrap_info = relay_wrap_info_t(last_wrap_info.evm_height(), last_wrap_info.elect_height(), cs_para.get_timestamp());
        //     relay_block = data::xrelay_block(last_relay_block.get_block_hash(), last_relay_block.get_block_height() + 1, cs_para.get_election_round(), cs_para.get_timestamp());
        // } else {
            // try build tx relay block
            uint64_t cur_evm_table_height = 0;
            std::vector<data::xeth_transaction_t> transactions;
            std::vector<data::xeth_receipt_t> receipts;
            auto result = get_relay_chain_mgr()->get_tx_cache_leader(last_wrap_info.evm_height(), cur_evm_table_height, transactions, receipts, RELAY_BLOCK_TXS_PACK_NUM_MAX);
            if (!result || cur_evm_table_height <= last_wrap_info.evm_height()) {
                xinfo("xtable_maker_t::build_relay_block_data_leader no cross txs and no new relay election. cs_para:%s,result:%d,cur evm h:%llu,last evm h:%llu",
                      cs_para.dump().c_str(),
                      result,
                      cur_evm_table_height,
                      last_wrap_info.evm_height());
                return false;
            }
            new_wrap_info = relay_wrap_info_t(cur_evm_table_height, last_wrap_info.elect_height(), last_wrap_info.poly_timestamp());
            relay_block = data::xrelay_block(
                last_relay_block.get_block_hash(), last_relay_block.get_block_height() + 1, cs_para.get_election_round(), cs_para.get_timestamp(), transactions, receipts);
        // }
    }
    relay_block.build_finish();
    xdbg("xtable_maker_t::build_relay_block_data_leader relay_block:%s", relay_block.dump().c_str());
    return true;
}

bool xtable_maker_t::build_relay_block_data_backup(const data::xblock_consensus_para_t & cs_para,
                                                   const data::xrelay_block & last_relay_block,
                                                   const relay_wrap_info_t & last_wrap_info,
                                                   const relay_wrap_info_t & new_wrap_info,
                                                   data::xrelay_block & relay_block) {
    bool new_elect = (last_wrap_info.elect_height() + 1 == new_wrap_info.elect_height());
    bool new_txs = (last_wrap_info.evm_height() < new_wrap_info.evm_height());
    bool poly_block = (last_wrap_info.poly_timestamp() < new_wrap_info.poly_timestamp());
    uint32_t type_num = (new_elect ? 1:0) + (new_txs ? 1:0) + (poly_block ? 1:0);
    if (type_num != 1) {
        xerror("xtable_maker_t::build_relay_block_data_backup invalid wrap info: elect:%llu:%llu,evm:%llu:%llu,poly_tm:%llu:%llu",
               last_wrap_info.elect_height(),
               new_wrap_info.elect_height(),
               last_wrap_info.evm_height(),
               new_wrap_info.evm_height(),
               last_wrap_info.poly_timestamp(),
               new_wrap_info.poly_timestamp());
        return false;
    }

    std::vector<data::xeth_transaction_t> transactions;
    std::vector<data::xeth_receipt_t> receipts;
    std::vector<data::xrelay_election_node_t> reley_election;
    if (new_elect) {
        auto ret = get_relay_chain_mgr()->get_elect_cache(new_wrap_info.elect_height(), reley_election);
        if (!ret) {
            xinfo("xtable_maker_t::build_relay_block_data_backup. get elect cache fail. height:%llu", new_wrap_info.elect_height());
            return false;
        }
        data::xrelay_election_group_t reley_election_group;
        reley_election_group.election_epochID = new_wrap_info.elect_height();
        reley_election_group.elections_vector = reley_election;
        relay_block = data::xrelay_block(
            last_relay_block.get_block_hash(), last_relay_block.get_block_height() + 1, cs_para.get_election_round(), cs_para.get_timestamp(), reley_election_group);
    } else if (poly_block) {
        if((cs_para.get_timestamp() < last_wrap_info.poly_timestamp() + XGET_ONCHAIN_GOVERNANCE_PARAMETER(max_relay_poly_interval) * 10) || last_relay_block.get_all_receipts().size() <= 0) {
            return false;
        }
        relay_block = data::xrelay_block(last_relay_block.get_block_hash(), last_relay_block.get_block_height() + 1, cs_para.get_election_round(), cs_para.get_timestamp());
    } else {
        auto ret = get_relay_chain_mgr()->get_tx_cache_backup(last_wrap_info.evm_height(), new_wrap_info.evm_height(), transactions, receipts);
        if (!ret) {
            return false;
        }
        relay_block = data::xrelay_block(
            last_relay_block.get_block_hash(), last_relay_block.get_block_height() + 1, cs_para.get_election_round(), cs_para.get_timestamp(), transactions, receipts);
    }
    relay_block.build_finish();
    xdbg("xtable_maker_t::build_relay_block_data_backup relay_block:%s", relay_block.dump().c_str());
    return true;
}

const std::string xeth_header_builder::build(const xblock_consensus_para_t & cs_para, const std::vector<txexecutor::xatomictx_output_t> & pack_txs_outputs) {
    auto fork_config = top::chain_fork::xtop_chain_fork_config_center::chain_fork_config();
    if (!top::chain_fork::xtop_chain_fork_config_center::is_forked(fork_config.eth_fork_point, cs_para.get_clock())) {
        return {};
    }
    
    std::error_code ec;
    uint64_t gas_used = 0;
    data::xeth_receipts_t eth_receipts;
    data::xeth_transactions_t eth_txs; 
    for (auto & txout : pack_txs_outputs) {
        if (txout.m_tx->get_tx_version() != data::xtransaction_version_3) {
            continue;
        }

        data::xeth_transaction_t ethtx = txout.m_tx->get_transaction()->to_eth_tx(ec);
        if (ec) {
            xerror("xeth_header_builder::build fail-to eth tx");
            continue;
        }

        auto & evm_result = txout.m_vm_output.m_tx_result;
        gas_used += evm_result.used_gas;
        data::enum_ethreceipt_status status = (evm_result.status == evm_common::xevm_transaction_status_t::Success) ? data::ethreceipt_status_successful : data::ethreceipt_status_failed;
        data::xeth_receipt_t eth_receipt(ethtx.get_tx_version(), status, gas_used, evm_result.logs);
        eth_receipt.create_bloom();
        eth_receipts.push_back(eth_receipt);
        eth_txs.push_back(ethtx);
    }

    data::xeth_header_t eth_header;
    data::xethheader_para_t header_para;
    header_para.m_gaslimit = cs_para.get_block_gaslimit();
    header_para.m_baseprice = cs_para.get_block_base_price();
    if (!cs_para.get_coinbase().empty()) {
        header_para.m_coinbase = common::xeth_address_t::build_from(cs_para.get_coinbase());
    }    
    data::xeth_build_t::build_ethheader(header_para, eth_txs, eth_receipts, eth_header);

    std::string _ethheader_str = eth_header.serialize_to_string();
    xdbg("xeth_header_builder::build ethheader txcount=%zu,headersize=%zu", eth_receipts.size(), _ethheader_str.size());
    return _ethheader_str;
}

bool xeth_header_builder::string_to_eth_header(const std::string & eth_header_str, data::xeth_header_t & eth_header) {
    std::error_code ec;
    eth_header.serialize_from_string(eth_header_str, ec);
    if (ec) {
        xerror("xeth_header_builder::string_to_eth_header decode fail");
        return false;
    }
    return true;
}

NS_END2
