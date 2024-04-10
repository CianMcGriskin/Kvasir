#ifndef KVASIR_NOTIFICATIONQUEUE_H
#define KVASIR_NOTIFICATIONQUEUE_H

#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <queue>
#include <mutex>
#include <condition_variable>

class NotificationQueue {
public:
    // Struct containing information for a notification
    struct Notification
    {
        std::string name;
        std::string reason;
        float confidence;
        std::string timestamp;

        Notification()
        {
            auto now = std::chrono::system_clock::now();

            // Convert to a time_t
            auto now_c = std::chrono::system_clock::to_time_t(now);

            // Convert to tm for formatting
            std::tm now_tm = *std::localtime(&now_c);

            // Format the timestamp as a string
            std::ostringstream oss;
            oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S");
            timestamp = oss.str();
        }
    };

    // Function used to push a notification to the queue
    void push(const Notification& notification);

    int size();

    void clear();

    // Function to wait for a notification and process it, poping it from the queue
    void wait_and_pop(Notification &notification);

private:
    std::mutex mutex;
    std::condition_variable conditionVar;
    std::queue<Notification> queue;
};

#endif //KVASIR_NOTIFICATIONQUEUE_H