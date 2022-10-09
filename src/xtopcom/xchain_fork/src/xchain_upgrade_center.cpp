// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "xchain_fork/xchain_upgrade_center.h"

#include "xconfig/xpredefined_configurations.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xchain_names.h"
#include "xvledger/xvblock.h"

#include <cinttypes>

namespace top {
    namespace chain_fork {

        xchain_fork_config_t xtop_chain_fork_config_center::m_fork_config;

#if defined(XCHAIN_FORKED_BY_DEFAULT)
#   if defined(XBUILD_CI)
        const uint32_t BLOCK_FORK_POINT = 6000000;
#   else
        const uint32_t BLOCK_FORK_POINT = 0;
#   endif

            xchain_fork_config_t  mainnet_chain_config{
                xfork_point_t{xfork_point_type_t::logic_time, 0, "block fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, BLOCK_FORK_POINT, "v3 block fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "tx v2 fee fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "partly remove confirm"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "add rsp id"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "inner table tx"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "enable eth shard"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "enable relay"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.6 version control"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.7 version control"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.7 sync protocal fork"},
            };

            // !!!change!!! fork time for galileo
            xchain_fork_config_t  testnet_chain_config{
                xfork_point_t{xfork_point_type_t::logic_time, 0, "block fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, BLOCK_FORK_POINT, "v3 block fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "tx v2 fee fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "partly remove confirm"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "add rsp id"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "inner table tx"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "enable eth shard"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "enable relay"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.6 version control"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.7 version control"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.7 sync protocal fork"},
           };

            xchain_fork_config_t default_chain_config {
                xfork_point_t{xfork_point_type_t::logic_time, 0, "block fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, BLOCK_FORK_POINT, "v3 block fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "tx v2 fee fork point"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "partly remove confirm"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "add rsp id"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "inner table tx"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "enable eth shard"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "enable relay"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.6 version control"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.7 version control"},
                xfork_point_t{xfork_point_type_t::logic_time, 0, "v1.7 sync protocal fork"},
            };
#else   // #if defined(XCHAIN_FORKED_BY_DEFAULT)
        xchain_fork_config_t  mainnet_chain_config{
            xfork_point_t{xfork_point_type_t::logic_time, 6859080, "block fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7126740, "v3 block fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7221960, "tx v2 fee fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7473960, "partly remove confirm"},
            xfork_point_t{xfork_point_type_t::logic_time, 7714440, "add rsp id"},
            xfork_point_t{xfork_point_type_t::logic_time, 7716060, "inner table tx"},// should later than "add rsp id"
            xfork_point_t{xfork_point_type_t::logic_time, 8224200, "enable eth shard"},// 2022-06-17 10:00:00
            xfork_point_t{xfork_point_type_t::logic_time, 8820360, "enable relay"},
            xfork_point_t{xfork_point_type_t::logic_time, 8820360, "v1.6 version control"},
            xfork_point_t{xfork_point_type_t::logic_time, 10000000, "v1.7 version control"},
            xfork_point_t{xfork_point_type_t::logic_time, 10000000, "v1.7 sync protocal fork"},
        };
        // !!!change!!! fork time for galileo
        xchain_fork_config_t  testnet_chain_config{
            xfork_point_t{xfork_point_type_t::logic_time, 6859080, "block fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7126740, "v3 block fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7221960, "tx v2 fee fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7473960, "partly remove confirm"},
            xfork_point_t{xfork_point_type_t::logic_time, 7714440, "add rsp id"},
            xfork_point_t{xfork_point_type_t::logic_time, 7716060, "inner table tx"},
            xfork_point_t{xfork_point_type_t::logic_time, 8224200, "enable eth shard"},
            xfork_point_t{xfork_point_type_t::logic_time, 8820360, "enable relay"},
            xfork_point_t{xfork_point_type_t::logic_time, 8820360, "v1.6 version control"},
            xfork_point_t{xfork_point_type_t::logic_time, 10000000, "v1.7 version control"},
            xfork_point_t{xfork_point_type_t::logic_time, 10000000, "v1.7 sync protocal fork"},
        };

        // !!!change!!! fork time for local develop net
        xchain_fork_config_t default_chain_config {
            xfork_point_t{xfork_point_type_t::logic_time, 6859080, "block fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7126740, "v3 block fork point"},
            xfork_point_t{xfork_point_type_t::logic_time, 7221960, "tx v2 fee fork point"},//2022-2-21 10:00:00
            xfork_point_t{xfork_point_type_t::logic_time, 7473960, "partly remove confirm"},
            xfork_point_t{xfork_point_type_t::logic_time, 7714440, "add rsp id"},
            xfork_point_t{xfork_point_type_t::logic_time, 7716060, "inner table tx"},
            xfork_point_t{xfork_point_type_t::logic_time, 8224200, "enable eth shard"},
            xfork_point_t{xfork_point_type_t::logic_time, 8820360, "enable relay"},
            xfork_point_t{xfork_point_type_t::logic_time, 8820360, "v1.6 version control"},
            xfork_point_t{xfork_point_type_t::logic_time, 10000000, "v1.7 version control"},
            xfork_point_t{xfork_point_type_t::logic_time, 10000000, "v1.7 sync protocal fork"},
        };
#endif  // #if defined(XCHAIN_FORKED_BY_DEFAULT)
        xchain_fork_config_t const & xtop_chain_fork_config_center::chain_fork_config() noexcept {
            if (top::config::chain_name_mainnet == XGET_CONFIG(chain_name)) {
                return mainnet_chain_config;
            } else if (top::config::chain_name_testnet == XGET_CONFIG(chain_name)) {
                return testnet_chain_config;
            }

            return default_chain_config;
        }

        bool xtop_chain_fork_config_center::is_forked(top::optional<xfork_point_t> const& fork_point, uint64_t target) noexcept {
            if (!fork_point.has_value()) {
                return false;
            }

            xdbg("xtop_chain_fork_config_center::is_forked target:%llu, fork point:%llu, %s", target, fork_point.value().point, fork_point->description.c_str());
            return  target >= fork_point.value().point;
        }

        bool xtop_chain_fork_config_center::is_block_forked(uint64_t target) noexcept {
            xchain_fork_config_t const & _fork_config = xtop_chain_fork_config_center::get_chain_fork_config();
            return  xtop_chain_fork_config_center::is_forked(_fork_config.v1_7_0_block_fork_point, target);
        }

        bool xtop_chain_fork_config_center::is_tx_forked_by_timestamp(uint64_t fire_timestamp) noexcept {
            xchain_fork_config_t const & _fork_config = xtop_chain_fork_config_center::get_chain_fork_config();
            auto clock = _fork_config.block_fork_point.value().point;
            auto clock_time_stamp = clock * 10 + base::TOP_BEGIN_GMTIME;
            return fire_timestamp >= clock_time_stamp;
        }

        void xtop_chain_fork_config_center::init() {
            if (top::config::chain_name_mainnet == XGET_CONFIG(chain_name)) {
                m_fork_config = mainnet_chain_config;
                xinfo("xtop_chain_fork_config_center::init mainnet config");
            } else if (top::config::chain_name_testnet == XGET_CONFIG(chain_name)) {
                m_fork_config = testnet_chain_config;
                xinfo("xtop_chain_fork_config_center::init testnet config");
            } else {
                m_fork_config = default_chain_config;
                xinfo("xtop_chain_fork_config_center::init default config");
            }
        }
    }
}
