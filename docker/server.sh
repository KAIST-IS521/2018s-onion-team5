#!/bin/bash
/root/onitok_client REST &
/root/onitok_client BROADCAST &
/root/onitok_client HEALTH &
/bin/bash
