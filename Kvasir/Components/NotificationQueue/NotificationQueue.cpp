#include "NotificationQueue.h"

// Pushes a notification to the queue and notifies waiting threads.
void NotificationQueue::push(const Notification& notification) {
    // Acquire a lock on the mutex to for thread-safe access to the queue.
    std::lock_guard<std::mutex> lock(mutex);

    // Add the notification to the queue.
    queue.push(notification);

    // This allows waiting threads to wake up and process the notification.
    conditionVar.notify_one();
}

// When a thread becomes available and then pops it from the queue.
void NotificationQueue::wait_and_pop(Notification& notification) {

    // Acquire a unique lock on the mutex
    std::unique_lock<std::mutex> lock(mutex);

    // Wait until the queue is not empty.
    while (queue.empty()) {
        conditionVar.wait(lock);
    }

    // Remove notification from queue after being processed
    notification = queue.front();
    queue.pop();
}

