// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xmbus/xevent.h"
#include "xdata/xblock.h"
#include "xstatistic/xbasic_size.hpp"
#include "xstatistic/xstatistic.h"
#include "xvnetwork/xaddress.h"

NS_BEG2(top, mbus)

class xevent_sync_executor_t : public xbus_event_t, public xstatistic::xstatistic_obj_face_t {
public:

    enum _minor_type_ {
        none,
        blocks,
        chain_snapshot,
        archive_blocks,
    };

    xevent_sync_executor_t(_minor_type_ mt = none,
            direction_type dir = to_listener,
            bool _sync = true) :
    xbus_event_t(xevent_major_type_sync_executor,
    (int) mt,
    dir,
    _sync),
    xstatistic::xstatistic_obj_face_t(xstatistic::enum_statistic_event_sync_executor) {
    }
    
    ~xevent_sync_executor_t() {statistic_del();}
    virtual int32_t get_class_type() const override {return xstatistic::enum_statistic_event_sync_executor;}
private:
    virtual int32_t get_object_size_real() const override {
        return sizeof(*this) + get_size(get_result_data());
    }
};

using xevent_sync_executor_ptr_t = xobject_ptr_t<xevent_sync_executor_t>;

class xevent_sync_response_blocks_t : public xevent_sync_executor_t {
public:

    xevent_sync_response_blocks_t(
            const std::vector<data::xblock_ptr_t> &_blocks,
            const vnetwork::xvnode_address_t& _self_address,
            const vnetwork::xvnode_address_t& _from_address,
            direction_type dir = to_listener,
            bool _sync = true) :
    xevent_sync_executor_t(xevent_sync_executor_t::blocks, dir, _sync),
    blocks(_blocks),
    self_address(_self_address),
    from_address(_from_address) {
    }

    std::vector<data::xblock_ptr_t> blocks;
    vnetwork::xvnode_address_t self_address;
    vnetwork::xvnode_address_t from_address;
};

class xevent_chain_snaphsot_t : public xevent_sync_executor_t {
public:
    xevent_chain_snaphsot_t(
            const std::string tbl_account_addr,
            const std::string & _chain_snapshot,
            uint64_t height,
            const vnetwork::xvnode_address_t& _self_address,
            const vnetwork::xvnode_address_t& _from_address,
            direction_type dir = to_listener,
            bool _sync = true) :
    xevent_sync_executor_t(xevent_sync_executor_t::chain_snapshot, dir, _sync),
    m_tbl_account_addr(tbl_account_addr),
    m_chain_snapshot(_chain_snapshot),
    m_height(height),
    self_address(_self_address),
    from_address(_from_address) {
    }
    std::string m_tbl_account_addr;
    std::string m_chain_snapshot;
    uint64_t m_height;
    vnetwork::xvnode_address_t self_address;
    vnetwork::xvnode_address_t from_address;
};
using xevent_sync_response_blocks_ptr_t = xobject_ptr_t<xevent_sync_response_blocks_t>;

class xevent_sync_archive_blocks_t : public xevent_sync_executor_t {
public:

    xevent_sync_archive_blocks_t(
            const std::vector<data::xblock_ptr_t> &_blocks,
            const vnetwork::xvnode_address_t& _self_address,
            const vnetwork::xvnode_address_t& _from_address,
            direction_type dir = to_listener,
            bool _sync = true) :
    xevent_sync_executor_t(xevent_sync_executor_t::archive_blocks, dir, _sync),
    blocks(_blocks),
    self_address(_self_address),
    from_address(_from_address) {
    }

    std::vector<data::xblock_ptr_t> blocks;
    vnetwork::xvnode_address_t self_address;
    vnetwork::xvnode_address_t from_address;
};
NS_END2
