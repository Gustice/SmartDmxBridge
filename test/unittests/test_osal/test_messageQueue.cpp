#include "message_queue.hpp"
#include <gtest/gtest.h>

struct MessageFrame {
    MessageFrame(int v, std::string msg) : value(v), message(msg) {}
    int value;
    std::string message;
};

extern std::string lastCall;
extern int64_t pItemToTransfer;

// Testing only the absolute basics here, to keep mock as simple as possible

TEST(OsalTestsMessageQueue, ConstructInstance) {
    MessageQueue<MessageFrame> eut(10);
    EXPECT_EQ("Constructor: queueSize=10 elementSize=8", lastCall);
}

TEST(OsalTestsMessageQueue, EnqueueOneMessage) {
    MessageQueue<MessageFrame> eut(10);
    
    auto e = std::make_unique<MessageFrame>(1, "one");
    const void *const pE = e.get();
    eut.enqueue(std::move(e));
    EXPECT_EQ(pItemToTransfer, (int64_t)pE);
}

TEST(OsalTestsMessageQueue, DequingOneMessage) {
    MessageQueue<MessageFrame> eut(10);
    
    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue();
    EXPECT_NE(nullptr, r.get());
    EXPECT_EQ(1, r->value);
    EXPECT_EQ("one", r->message);
}

