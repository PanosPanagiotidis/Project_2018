# Project 2018


Implementation of Radix Hash Join Algorithm on two different tables.

Initial implementation is broken down in three stages.

- Breaking down the data in buckets.
- Creating an index for one of the two tables.
- Getting the results using the index.

## Breaking Down the data in buckets.

Initially the arrays are generated using rand().
Ids go from 1 to a Row argument.
Payloads are random.

Choosing an *N* large enough,so that we create *2^N* buckets of data.Each bucket can fit in the cache memory.

Those buckets contain reordered data from the payload arrays.Reordering of the data is based on the result of a hashing function by keeping the last *n* significant bytes.

After the buckets are created,we create an index on a table,preferably the one with less rows.

## Creating an index for one of the two tables.

Each non-empty bucket has an index. Each index is comprised of a static hash table and an array of integers (chain array) with a size equal to the size of the bucket it corresponds to.

The hash table bucket that a payload hashes to contains the bucket index of the last instance of a payload that hashes to that value.

Thus, by looking a payload up using the index, all bucket entries that hash to the same value as the payload can be returned efficiently.


## Compiling and Running

To compile and run the program using the harness, simply run the bash script:

```
./run.sh

```

need to chmode run.sh and runTestharness.sh
