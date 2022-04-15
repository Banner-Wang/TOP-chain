// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <mutex>
#include "json/json.h"
#include "xpre_request_handler.h"
#include "crossguid/Guid.hpp"
#include "xpre_request_handler_mgr.h"
#include "xrpc/xerror/xrpc_error.h"
#include "xrpc/xrpc_signature.h"
#include "xrpc/prerequest/xpre_request_handler_server.h"

NS_BEG2(top, xrpc)

#define PRE_REQUEST_REGISTER_V1(func_name) register_method(pair<string, string>{"1.0", #func_name}, std::bind(&xpre_request_token_handler::func_name, this, _1))
using top::xChainRPC::xrpc_signature;

xpre_request_token_handler::xpre_request_token_handler() {
    PRE_REQUEST_REGISTER_V1(requestToken);
    PRE_REQUEST_REGISTER_V1(getEstimateGas);
    // PRE_REQUEST_REGISTER_V1(create_account);
    // PRE_REQUEST_REGISTER_V1(account);
}

void xpre_request_token_handler::requestToken(xpre_request_data_t & request) {
    std::string guid = xg::newGuid().str();
    std::string secret_key = xg::newGuid().str();
    auto account_address = request.get_request_value("target_account_addr");
    xJson::Value result_json;
    result_json[xrpc_signature::secretkey_key_] = secret_key;                   // secret key
    result_json[xrpc_signature::keyid_key_] = guid;                             // section key id
    result_json[xrpc_signature::method_key_] = xrpc_signature::method_value_;   // sign method
    result_json[xrpc_signature::version_key_] = xrpc_signature::version_value_; // sign version
    request.m_response_json["data"] = result_json;
    request.m_finish = true;
    edge_account_info account_info;
    account_info.m_token = guid;
    account_info.m_secretid = secret_key;
    account_info.m_record_time_point = steady_clock::now();
    //std::lock_guard<std::mutex> lock(pre_request_service<>::m_mutex);
    pre_request_service<>::m_account_info_map.put(account_address, account_info);
    //get_service()->m_account_token_map[account_address] = ;
    //get_service()->m_account_secretid_map[account_address] = secret_key;
}

// void xpre_request_token_handler::create_account(xpre_request_data_t & request) {
// }

// void xpre_request_token_handler::account(xpre_request_data_t & request) {
// }

void xpre_request_token_handler::getEstimateGas(xpre_request_data_t & request) {
    xJson::Value result_json;
    xJson::Value req_json;
    auto body = request.get_request_value("body");
    xJson::Reader reader;
    reader.parse(body, req_json);
    auto tx = top::data::xtx_factory::create_tx(static_cast<data::enum_xtransaction_version>(req_json["params"]["tx_structure_version"].asUInt()));
    tx->construct_from_json(req_json["params"]);
    tx->set_len();
    top::data::xcons_transaction_ptr_t trans = make_object_ptr<top::data::xcons_transaction_t>(tx.get());
    result_json["EstimateGas"] = top::txexecutor::xtransaction_fee_t::calc_tgas_usage(trans, false);
    request.m_response_json["data"] = result_json;
    request.m_finish = true;
}

NS_END2
