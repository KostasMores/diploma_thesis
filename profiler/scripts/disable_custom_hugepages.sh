#!/bin/bash

echo never >/sys/kernel/mm/transparent_hugepage/hugepages-16kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-32kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-64kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-128kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-256kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-512kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-1024kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-2048kB/enabled

echo "Hugepages Disabled"
