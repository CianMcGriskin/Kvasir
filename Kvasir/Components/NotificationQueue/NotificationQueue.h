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

    void push(const Notification& notification);

    void wait_and_pop(Notification &notification);

private:
    std::mutex mutex;
    std::condition_variable conditionVar;
    std::queue<Notification> queue;
};

#endif //KVASIR_NOTIFICATIONQUEUE_H