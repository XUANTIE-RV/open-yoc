/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef POSTO_DISCOVERY_DISCOVERY_H_
#define POSTO_DISCOVERY_DISCOVERY_H_

namespace posto {
namespace discovery {

enum ActionType {
  ACTION_JOIN = 1,
  ACTION_LEAVE = 2,
};

inline const char* Action2Name(ActionType type) {
  switch (type) {
  case ACTION_JOIN:
    return "JOIN";
  case ACTION_LEAVE:
    return "LEAVE";
  default:
    return "N/A";
  }
}

}  // namespace discovery
}  // namespace posto

#endif  // POSTO_DISCOVERY_DISCOVERY_H_
