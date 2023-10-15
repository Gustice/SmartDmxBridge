#include "message_queue.hpp"
#include <unity.h>

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

struct MessageFrame {
    MessageFrame(int v, std::string msg) : value(v), message(msg) {}
    int value;
    std::string message;
};

void setUp(void) {}

void tearDown(void) {}

void enqueue_then_dequeue(void) {
    MessageQueue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue();
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    TEST_ASSERT_TRUE(r->message == "one");
}

void enqueue_then_dequeue_multiple(void) {
    MessageQueue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));
    e = std::make_unique<MessageFrame>(2, "two");
    eut.enqueue(std::move(e));
    e = std::make_unique<MessageFrame>(3, "three");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue();
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    TEST_ASSERT_TRUE(r->message == "one");
    r = eut.dequeue();
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(2, r->value);
    TEST_ASSERT_TRUE(r->message == "two");
    r = eut.dequeue();
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(3, r->value);
    TEST_ASSERT_TRUE(r->message == "three");
}

void enqueue_cyclically(void) {
    constexpr int Size = 10;
    MessageQueue<MessageFrame> eut(Size);
    for (size_t i = 0; i < Size * 2; i++) {
        auto e = std::make_unique<MessageFrame>(i, "Some message");
        eut.enqueue(std::move(e));

        auto r = eut.dequeue();
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_EQUAL(i, r->value);
    }
    TEST_ASSERT_TRUE(true);
}

static void senderTask(void *arg) {
    auto &q = *static_cast<MessageQueue<MessageFrame> *>(arg);
    size_t i = 0;
    while (q.isActive()) {
        auto e = std::make_unique<MessageFrame>(i++, "Some message");
        q.enqueue(std::move(e));
        vTaskDelay(1);
    }
}

void enqueue_task_based(void) {
    constexpr int Size = 10;
    MessageQueue<MessageFrame> eut(Size);
    xTaskCreate(senderTask, "displayTask", 8192, &eut, 10, NULL);
    for (size_t i = 0; i < Size * 2; i++) {
        auto r = eut.dequeue();
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_EQUAL(i, r->value);
    }
    eut.signalClose();
    TEST_ASSERT_TRUE(true);
}

void enqueue_till_overflow(void) {
    MessageQueue<MessageFrame> eut(3);

    auto e = std::make_unique<MessageFrame>(1, "one");
    auto r = eut.enqueue(std::move(e));
    TEST_ASSERT_EQUAL(nullptr, r.get());

    e = std::make_unique<MessageFrame>(2, "two");
    r = eut.enqueue(std::move(e));
    TEST_ASSERT_EQUAL(nullptr, r.get());

    e = std::make_unique<MessageFrame>(3, "three");
    r = eut.enqueue(std::move(e));
    TEST_ASSERT_EQUAL(nullptr, r.get());

    e = std::make_unique<MessageFrame>(4, "four");
    auto pE = e.get();
    r = eut.enqueue(std::move(e));
    TEST_ASSERT_NOT_EQUAL(nullptr, r.get());
    // also return old element ... 
    TEST_ASSERT_EQUAL(pE, r.get());
}

void enqueue_then_dequeue_to_many(void) {
    MessageQueue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));
    e = std::make_unique<MessageFrame>(2, "two");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue();
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    r = eut.dequeue();
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(2, r->value);
    r = eut.dequeue(0); // no waiting
    TEST_ASSERT_EQUAL(nullptr, r.get());
}

void app_main(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(enqueue_then_dequeue);
    RUN_TEST(enqueue_then_dequeue_multiple);
    RUN_TEST(enqueue_cyclically);
    RUN_TEST(enqueue_till_overflow);
    RUN_TEST(enqueue_then_dequeue_to_many);
    RUN_TEST(enqueue_task_based);
    UNITY_END(); // stop unit testing
}
