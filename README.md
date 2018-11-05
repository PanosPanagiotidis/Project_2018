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

Choosing an N large enough,so that we create 2^N buckets of data.Each bucket can fit in the cache memory.

Those buckets contain reordered data from the payload arrays.Reordering of the data is based on the result of a hashing function by keeping the last *n* significant bytes.

After the buckets are created,we create an index on a table,preferably the one with less rows.

Each index is comprised of 2 arrays,