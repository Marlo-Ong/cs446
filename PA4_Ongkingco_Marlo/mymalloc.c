#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MBLOCK_HEADER_SZ offsetof(mblock_t, payload)

// Type definitions
typedef struct _mblock_t
{
    struct _mblock_t *prev;
    struct _mblock_t *next;
    size_t size;
    int status;
    void *payload;
} mblock_t;

typedef struct _mlist_t
{
    mblock_t *head;
} mlist_t;

// Function definitions
void *mymalloc(size_t size);
void myfree(void *ptr);

mblock_t *findLastMemlistBlock(void);
mblock_t *findFreeBlockOfSize(size_t size);
void splitBlockAtSize(mblock_t *block, size_t newSize);
void coallesceBlockPrev(mblock_t *freedBlock);
void coallesceBlockNext(mblock_t *freedBlock);
mblock_t *growHeapBySize(size_t size);

void printMemList(const mblock_t *headptr);

// Global memory list
mlist_t mlist;

int main(int argc, char *argv[])
{
    // Initialize memory list
    mlist.head = NULL;

    // Test code
    void *p1 = mymalloc(10);
    void *p2 = mymalloc(100);
    void *p3 = mymalloc(200);
    void *p4 = mymalloc(500);
    myfree(p3);
    p3 = NULL;
    myfree(p2);
    p2 = NULL;
    void *p5 = mymalloc(150);
    void *p6 = mymalloc(500);
    myfree(p4);
    p4 = NULL;
    myfree(p5);
    p5 = NULL;
    myfree(p6);
    p6 = NULL;
    myfree(p1);
    p1 = NULL;

    return 0;
}

void *mymalloc(size_t size)
{
    printMemList(mlist.head);

    // Find a free block. If there is none, add a block to the heap.
    mblock_t *curr = findFreeBlockOfSize(size);
    if (curr == NULL)
    {
        curr = growHeapBySize(size);
    }
    else
    {
        splitBlockAtSize(curr, size);
    }

    // Change free/allocated status.
    curr->status = 1;

    return &(curr->payload);
}

void myfree(void *ptr)
{
}

mblock_t *findLastMemlistBlock(void)
{
    mblock_t *last = mlist.head;
    if (last == NULL)
    {
        return NULL;
    }
    while (last->next != NULL)
    {
        last = last->next;
    }
    return last;
}

mblock_t *findFreeBlockOfSize(size_t size)
{
    // Traverse memory list to find first available
    // free block that can fit the requested size.
    mblock_t *curr = mlist.head;
    while (curr != NULL)
    {
        if (curr->status == 0 && curr->size >= size)
        {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

void splitBlockAtSize(mblock_t *block, size_t newSize)
{
    // If the block is an exact fit, no need to split.
    if (block->size == newSize)
    {
        return;
    }

    size_t remaining_size = block->size - newSize;

    // Only split if the remaining size is large enough for a new block.
    if (remaining_size <= sizeof(mblock_t) + 1)
    {
        return;
    }

    // Create a block at the address following the used space.
    char *addr_remaining_blocck = (char *)block + MBLOCK_HEADER_SZ + newSize;
    mblock_t *remaining_block = (mblock_t *)addr_remaining_blocck;

    // Initialize the new free block
    remaining_block->prev = block;
    remaining_block->next = block->next;
    remaining_block->size = remaining_size - MBLOCK_HEADER_SZ;
    remaining_block->status = 0;
    remaining_block->payload = (char *)remaining_block + MBLOCK_HEADER_SZ;

    // Fix links in the original block and the next block
    if (block->next != NULL)
    {
        block->next->prev = remaining_block;
    }

    block->next = remaining_block;
    block->size = newSize;
}

void coallesceBlockPrev(mblock_t *freedBlock)
{
}

void coallesceBlockNext(mblock_t *freedBlock)
{
}

mblock_t *growHeapBySize(size_t size)
{
    // Increment break and reinterpret it as a memory block.
    size_t total_size = size + MBLOCK_HEADER_SZ;
    mblock_t *block = (mblock_t *)sbrk(total_size);

    // Set block values.
    block->prev = NULL;
    block->next = NULL;
    block->size = size;
    block->status = 0;
    block->payload = (char *)block + MBLOCK_HEADER_SZ;

    // Update memory freelist.
    if (!mlist.head)
    {
        mlist.head = block;
    }
    else
    {
        mblock_t *last = findLastMemlistBlock();
        last->next = block;
        block->prev = last;
    }

    return block;
}

void printMemList(const mblock_t *head)
{
    const mblock_t *p = head;

    size_t i = 0;
    while (p != NULL)
    {
        printf("[%ld] p: %p\n", i, (void *)p);
        printf("[%ld] p->size: %ld\n", i, p->size);
        printf("[%ld] p->status: %s\n", i, p->status > 0 ? "allocated" : "free");
        printf("[%ld] p->prev: %p\n", i, (void *)p->prev);
        printf("[%ld] p->next: %p\n", i, (void *)p->next);
        printf("___________________________\n");
        ++i;
        p = p->next;
    }
    printf("===========================\n");
}
