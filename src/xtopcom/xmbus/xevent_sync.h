// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <memory>
#include <string>
#include "xmbus/xevent.h"
#include "xstatistic/xbasic_size.hpp"
#include "xstatistic/xstatistic.h"

NS_BEG2(top, mbus)

class xevent_sync_t : public xbus_event_t, public xstatistic::xstatistic_obj_face_t {
public:

    enum _minor_type_ {
        type_complete,
    };

    xevent_sync_t(_minor_type_ sub_type,
            direction_type dir = to_listener, bool _sync = true)
    : xbus_event_t(xevent_major_type_sync, (int) sub_type, dir, _sync), xstatistic::xstatistic_obj_face_t(xstatistic::enum_statistic_event_sync) {
    }

    ~xevent_sync_t() {statistic_del();}

    virtual int32_t get_class_type() const override {return xstatistic::enum_statistic_event_sync;}
private:
    virtual int32_t get_object_size_real() const override {
        return sizeof(*this) + get_size(get_result_data());
    }
};

using xevent_sync_ptr_t = xobject_ptr_t<xevent_sync_t>;

class xevent_sync_complete_t : public xevent_sync_t {
public:

    xevent_sync_complete_t(
            const std::string &_address,
            direction_type dir = to_listener,
            bool _sync = true)
    : xevent_sync_t(type_complete, dir, _sync)
    , address(_address) {
    }

    std::string address;
};

using xevent_sync_complete_ptr_t = xobject_ptr_t<xevent_sync_complete_t>;

NS_END2
