#pragma once

typedef bool lock_t;

void acquire_lock(lock_t *lock);
void release_lock(lock_t *lock);