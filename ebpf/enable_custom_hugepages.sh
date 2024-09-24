#!/bin/bash

echo always >/sys/kernel/mm/transparent_hugepage/hugepages-16kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-32kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-64kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-128kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-256kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-512kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-1024kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-2048kB/enabled