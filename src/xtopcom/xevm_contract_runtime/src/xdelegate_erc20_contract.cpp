// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xevm_contract_runtime/sys_contract/xevm_erc20_contract.h"

#include "xbasic/endianness.h"
#include "xcommon/xaccount_address.h"
#include "xcommon/xeth_address.h"
#include "xevm_common/common_data.h"

NS_BEG4(top, contract_runtime, evm, sys_contract)



bool xtop_evm_erc20_sys_contract::execute(xbytes_t input,
                                          uint64_t target_gas,
                                          sys_contract_context const & context,
                                          bool is_static,
                                          observer_ptr<statectx::xstatectx_face_t> state_ctx,
                                          sys_contract_precompile_output & output,
                                          sys_contract_precompile_error & err) {
    // ERC20 method ids:
    //--------------------------------------------------
    // decimals()                            => 313ce567
    // totalSupply()                         => 18160ddd
    // balanceOf(address)                    => 70a08231
    // transfer(address,uint256)             => a9059cbb
    // transferFrom(address,address,uint256) => 23b872dd
    // approve(address,uint256)              => 095ea7b3
    // allowance(address,address)            => dd62ed3e
    // approveTOP(bytes32,uint64)            => 24655e23
    //--------------------------------------------------
#if defined(__LITTLE_ENDIAN__)
    constexpr uint32_t method_id_decimals{0x67e53c31};
    constexpr uint32_t method_id_total_supply{0xdd0d1618};
    constexpr uint32_t method_id_balance_of{0x3182a070};
    constexpr uint32_t method_id_transfer{0xbb9c05a9};
    constexpr uint32_t method_id_transfer_from{0xdd72b823};
    constexpr uint32_t method_id_approve{0xb3a75e09};
    constexpr uint32_t method_id_allowance{0x3eed62dd};
#elif defined(__BIG_ENDIAN__)
    constexpr uint32_t method_id_decimals{0x313ce567};
    constexpr uint32_t method_id_total_supply{0x18160ddd};
    constexpr uint32_t method_id_balance_of{0x70a08231};
    constexpr uint32_t method_id_transfer{0xa9059cbb};
    constexpr uint32_t method_id_transfer_from{0x23b872dd};
    constexpr uint32_t method_id_approve{0x095ea7b3};
    constexpr uint32_t method_id_allowance{0xdd62ed3e};
#else
#    error "I don't know what architecture this is!"
#endif
    assert(state_ctx);

    // erc20_uuid (1 byte) | erc20_method_id (4 bytes) | parameters (depends)
    if (input.size() < 5) {
        err.fail_status = precompile_error::Fatal;
        err.minor_status = precompile_error_ExitFatal::Other;

        return false;
    }

    common::xtoken_id_t const erc20_token_id{top::from_byte<common::xtoken_id_t>(input.front())};

    xbytes_t const data{std::next(std::begin(input), 1), std::end(input)};
    xbytes_t const method_id_bytes{std::begin(data), std::next(std::begin(data), 4)};
    xbytes_t const parameters{std::next(std::begin(data), 4), std::end(data)};

    uint32_t method_id;
    std::memcpy(&method_id, method_id_bytes.data(), 4);

    switch (method_id) {
    case method_id_decimals: {
        uint64_t const decimals_gas_cost = 2535;

        if (!parameters.empty()) {
            err.cost = target_gas;
            err.fail_status = Error;

            return false;
        }

        if (target_gas < decimals_gas_cost) {
            err.cost = target_gas;
            err.fail_status = Error;

            return false;
        }

        xbytes_t decimals;
        decimals.resize(32);
        decimals[31] = static_cast<uint8_t>(18);

        output.exit_status = Returned;
        output.cost = 2535;
        output.output = decimals;

        return true;
    }

    case method_id_total_supply: {
        if (!parameters.empty()) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        evm_common::u256 supply{0};
        output.exit_status = Returned;
        output.cost = 0;
        output.output = top::to_bytes(supply);

        return true;
    }

    case method_id_balance_of: {
        if (parameters.size() != 32) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const prefix{std::begin(parameters), std::next(std::begin(parameters), 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const account_address_bytes{std::next(std::begin(parameters), 12), std::next(std::begin(parameters), 32)};
        std::error_code ec;
        common::xeth_address_t const eth_address{common::xeth_address_t::build_from(account_address_bytes, ec)};
        assert(!ec);

        auto state = state_ctx->load_unit_state(common::xaccount_address_t::build_from(eth_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec).vaccount());

        evm_common::u256 value{0};
        switch (erc20_token_id) {
        case common::xtoken_id_t::top: {
            value = state->balance();
            break;
        }

        case common::xtoken_id_t::usdt: {
            value = state->tep_token_balance("USDT");
            break;
        }

        case common::xtoken_id_t::usdc: {
            value = state->tep_token_balance("USDC");
            break;
        }

        default:
            assert(false);
            err.cost = target_gas / 2;
            err.fail_status = Revert;
            err.output = top::to_bytes(value);

            return false;
        }

        output.cost = 3268;
        output.exit_status = Returned;
        output.output = top::to_bytes(value);

        return true;
    }

    case method_id_transfer: {
        if (parameters.size() != 32 * 2) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const prefix{std::begin(parameters), std::next(std::begin(parameters), 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const to_account_address_bytes{std::next(std::begin(parameters), 12), std::next(std::begin(parameters), 32)};
        xbytes_t const value_bytes{std::next(std::begin(parameters), 32), std::next(std::begin(parameters), 32 + 32)};

        std::error_code ec;

        common::xeth_address_t recipient_address = common::xeth_address_t::build_from(to_account_address_bytes, ec);
        assert(!ec);
        common::xaccount_address_t recipient_account_address =
            common::xaccount_address_t::build_from(recipient_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec);
        assert(!ec);

        evm_common::u256 const value = top::from_bytes<evm_common::u256>(value_bytes);

        auto sender_state = state_ctx->load_unit_state(context.caller.vaccount());
        auto recver_state = state_ctx->load_unit_state(recipient_account_address.vaccount());

        xbytes_t result(32, 0);
        sender_state->transfer(erc20_token_id, top::make_observer(recver_state.get()), value, ec);

        if (!ec) {
            auto contract_address = common::xeth_address_t::build_from(context.address, ec);
            assert(!ec);
            auto caller_address = common::xeth_address_t::build_from(context.caller, ec);
            assert(!ec);

            evm_common::xevm_log_t log;
            log.address = top::to_string(contract_address.to_h160());
            assert(log.address.size() == 20);
            log.data = top::to_string(value);
            assert(log.data.size() == 32);
            log.topics.push_back(top::to_string(evm_common::fromHex("0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef", evm_common::WhenError::Throw)));
            assert(log.topics.back().size() == 32);
            log.topics.push_back(top::to_string(caller_address.to_h256()));
            assert(log.topics.back().size() == 32);
            log.topics.push_back(top::to_string(recipient_address.to_h256()));
            assert(log.topics.back().size() == 32);

            result[31] = 1;

            output.cost = 37839;
            output.exit_status = Returned;
            output.output = result;
            output.logs.push_back(log);
        } else {
            err.cost = 809;
            err.fail_status = Revert;
            // err.minor_status;
            err.output = result;
        }

        return !ec;
    }

    case method_id_transfer_from: {
        if (parameters.size() != 32 * 3) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t prefix{std::begin(parameters), std::next(std::begin(parameters), 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        prefix = xbytes_t{std::next(std::begin(parameters), 32), std::next(std::begin(parameters), 32 + 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const owner_account_address_bytes{std::next(std::begin(parameters), 12), std::next(std::begin(parameters), 32)};
        xbytes_t const to_account_address_bytes{std::next(std::begin(parameters), 32 + 12), std::next(std::begin(parameters), 32 + 32)};
        xbytes_t const value_bytes{std::next(std::begin(parameters), 32 * 2), std::next(std::begin(parameters), 32 * 3)};

        std::error_code ec;
        common::xeth_address_t owner_address = common::xeth_address_t::build_from(owner_account_address_bytes, ec);
        assert(!ec);

        common::xeth_address_t recipient_address = common::xeth_address_t::build_from(to_account_address_bytes, ec);
        assert(!ec);

        common::xaccount_address_t owner_account_address = common::xaccount_address_t::build_from(owner_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec);
        assert(!ec);
        common::xaccount_address_t recipient_account_address =
            common::xaccount_address_t::build_from(recipient_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec);
        assert(!ec);
        evm_common::u256 value = top::from_bytes<evm_common::u256>(value_bytes);

        xbytes_t result(32, 0);
        auto owner_state = state_ctx->load_unit_state(owner_account_address.vaccount());
        owner_state->update_allowance(erc20_token_id, context.caller, value, data::xallowance_update_op_t::decrease, ec);
        if (!ec) {
            auto recver_state = state_ctx->load_unit_state(recipient_account_address.vaccount());
            owner_state->transfer(erc20_token_id, top::make_observer(recver_state.get()), value, ec);
            if (!ec) {
                result[31] = 1;
            }
        }

        if (!ec) {
            auto contract_address = common::xeth_address_t::build_from(context.address, ec);
            assert(!ec);

            evm_common::xevm_log_t log;
            log.address = top::to_string(contract_address.to_h160());
            assert(log.address.size() == 20);
            log.data = top::to_string(value);
            assert(log.data.size() == 32);
            log.topics.push_back(top::to_string(evm_common::fromHex("0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef", evm_common::WhenError::Throw)));
            assert(log.topics.back().size() == 32);
            log.topics.push_back(top::to_string(owner_address.to_h256()));
            assert(log.topics.back().size() == 32);
            log.topics.push_back(top::to_string(recipient_address.to_h256()));
            assert(log.topics.back().size() == 32);

            result[31] = 1;

            output.cost = 37839;
            output.exit_status = Returned;
            output.output = result;
            output.logs.push_back(log);
        } else {
            err.cost = 1;
            err.fail_status = Revert;
            err.output = result;
        }

        return !ec;
    }

    case method_id_approve: {
        if (parameters.size() != 32 * 2) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const prefix{std::begin(parameters), std::next(std::begin(parameters), 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const spender_account_bytes{std::next(std::begin(parameters), 12), std::next(std::begin(parameters), 32)};
        xbytes_t const amount_bytes{std::next(std::begin(parameters), 32), std::next(std::begin(parameters), 32 + 32)};

        std::error_code ec;
        common::xeth_address_t spender_address = common::xeth_address_t::build_from(spender_account_bytes, ec);
        assert(!ec);

        common::xaccount_address_t spender_account_address = common::xaccount_address_t::build_from(spender_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec);
        assert(!ec);
        evm_common::u256 amount = top::from_bytes<evm_common::u256>(amount_bytes);

        xbytes_t result(32, 0);

        auto sender_state = state_ctx->load_unit_state(context.caller.vaccount());
        sender_state->approve(erc20_token_id, spender_account_address, amount, ec);

        if (!ec) {
            auto contract_address = common::xeth_address_t::build_from(context.address, ec);
            assert(!ec);
            auto caller_address = common::xeth_address_t::build_from(context.caller, ec);
            assert(!ec);

            evm_common::xevm_log_t log;
            log.address = top::to_string(contract_address.to_h160());
            assert(log.address.size() == 20);
            log.data = top::to_string(amount);
            assert(log.data.size() == 32);
            log.topics.push_back(top::to_string(evm_common::fromHex("0x8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925", evm_common::WhenError::Throw)));
            assert(log.topics.back().size() == 32);
            log.topics.push_back(top::to_string(caller_address.to_h256()));
            assert(log.topics.back().size() == 32);
            log.topics.push_back(top::to_string(spender_address.to_h256()));
            assert(log.topics.back().size() == 32);

            result[31] = 1;

            output.cost = 32007;
            output.exit_status = Returned;
            output.output = result;
            output.logs.push_back(log);
        } else {
            err.cost = 32007;
            err.fail_status = Revert;
            err.output = result;
        }

        return !ec;
    }

    case method_id_allowance: {
        if (parameters.size() != 32 * 2) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t prefix{std::begin(parameters), std::next(std::begin(parameters), 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        prefix = xbytes_t{std::next(std::begin(parameters), 32), std::next(std::begin(parameters), 32 + 12)};
        if (std::any_of(std::begin(prefix), std::end(prefix), [](xbyte_t byte) { return byte != 0; })) {
            err.fail_status = precompile_error::Fatal;
            err.minor_status = precompile_error_ExitFatal::Other;

            return false;
        }

        xbytes_t const owner_account_bytes{std::next(std::begin(parameters), 12), std::next(std::begin(parameters), 32)};
        xbytes_t const spender_account_bytes{std::next(std::begin(parameters), 32 + 12), std::next(std::begin(parameters), 32 + 32)};

        std::error_code ec;
        common::xeth_address_t owner_address = common::xeth_address_t::build_from(owner_account_bytes, ec);
        if (ec) {
            err.cost = 1;
            err.fail_status = Revert;
            return false;
        }

        common::xeth_address_t spender_address = common::xeth_address_t::build_from(spender_account_bytes, ec);
        if (ec) {
            err.cost = 1;
            err.fail_status = Revert;
            return false;
        }

        common::xaccount_address_t owner_account_address = common::xaccount_address_t::build_from(owner_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec);
        assert(!ec);
        common::xaccount_address_t spender_account_address = common::xaccount_address_t::build_from(spender_address, base::enum_vaccount_addr_type_secp256k1_evm_user_account, ec);
        assert(!ec);

        auto owner_state = state_ctx->load_unit_state(owner_account_address.vaccount());
        auto amount = owner_state->allowance(erc20_token_id, spender_account_address, ec);
        if (ec) {
            // internal_write_register(register_id, top::to_bytes(evm_common::u256{0}));
        } else {
            // internal_write_register(register_id, top::to_bytes(amount));
        }

        output.cost = 1;
        output.output = top::to_bytes(amount);
        output.exit_status = Returned;

        return true;
    }

    default: {
        assert(false);
        err.cost = 1;
        err.fail_status = Error;

        return false;
    }
    }
}

NS_END4
