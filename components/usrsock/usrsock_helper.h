/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_helper.h
 * @brief    user sock help functions header
 * @version  V1.0
 * @date     15. Mar 2020
 ******************************************************************************/

#ifndef USRSOCK_HELPER_H
#define USRSOCK_HELPER_H

struct hostent *gethostbyname_impl(const char *name);
void freeaddrinfo_impl(struct addrinfo *ai);
int getaddrinfo_impl(const char *hostname, const char *servname,
                     const struct addrinfo *hint, struct addrinfo **res);

#endif
