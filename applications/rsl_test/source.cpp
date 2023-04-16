#define RYTHE_VALIDATE

#include <rsl/delegate>
#include <rsl/buffered_string>
#include <rsl/time>

#include <rsl/math>

#include <iostream>
#include <chrono>

static size_t counter;

static void func() {
    counter++;
}

struct Object {
    std::vector<size_t> ints;

    void memberFunc() {
        for (size_t& i : ints) {
            i++;
            counter += i;
        }
    }

    void constMemberFunc() const {

        for (size_t i : ints) {
            counter += i;
        }
    }
};

static std::vector<size_t> createVec(size_t idx) {
    std::vector<size_t> vec;
    for (size_t i = idx + 20; i > idx; i--) {
        vec.push_back(i);
    }
    return vec;
}

void delegate_test(size_t i) {
    rsl::multicast_delegate<void()> del;

    {
        del = &func;
    }

    del();

    counter = 0;

    {
        del = []() {
            counter++;
        };
    }

    del();

    counter = 0;

    {
        std::vector<size_t> ints = createVec(i);
        del = [ints]() {
            for (size_t i : ints) {
                i++;
                counter += i;
            }
        };
    }

    del();

    counter = 0;

    {
        Object obj{ createVec(i) };
        {
            del.assign<Object, &Object::memberFunc>(obj);
        }

        del();
    }

    counter = 0;

    {
        const Object obj{ createVec(i) };
        {
            del.assign<Object, &Object::constMemberFunc>(obj);
        }

        del();
    }

    counter = 0;

    for (size_t j = 0; j < 10000; j++) {
        del();
    }
}

void stl_test(size_t i) {
    std::function<void()> del;

    {
        del = &func;
    }

    del();

    counter = 0;

    {
        del = []() {
            counter++;
        };
    }

    del();

    counter = 0;

    {
        std::vector<size_t> ints = createVec(i);
        del = [ints]() {
            for (size_t i : ints) {
                i++;
                counter += i;
            }
        };
    }

    del();

    counter = 0;

    {
        Object obj{ createVec(i) };
        {
            auto mf = std::mem_fn(&Object::memberFunc);
            del = std::bind(mf, &obj);
        }

        del();
    }

    counter = 0;

    {
        const Object obj{ createVec(i) };
        {
            auto mf = std::mem_fn(&Object::constMemberFunc);
            del = std::bind(mf, const_cast<Object*>(&obj));
        }

        del();
    }

    counter = 0;

    for (size_t j = 0; j < 10000; j++) {
        del();
    }
}

int main() {
    constexpr size_t ITERATIONS = 10000;
    using clock = std::chrono::high_resolution_clock;
    {
        auto start = clock::now();

        for (size_t i = 0; i < ITERATIONS; i++) {
            delegate_test(i);
        }

        auto elapsed = (clock::now() - start);
        std::cout << counter << std::endl;
        std::cout << (std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(elapsed).count() / ITERATIONS) << "\t" << elapsed.count() << std::endl;
        counter = 0;
    }

    {
        auto start = clock::now();

        for (size_t i = 0; i < ITERATIONS; i++) {
            stl_test(i);
        }

        auto elapsed = (clock::now() - start);
        std::cout << counter << std::endl;
        std::cout << (std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(elapsed).count() / ITERATIONS) << "\t" << elapsed.count() << std::endl;
        counter = 0;
    }

    rsl::stopwatch<rsl::fast_time, std::chrono::system_clock> timer;
    rsl::timer accurateTimer;

    rsl::time_point startTime = timer.start_point();

    std::cout << "timer started at: " << startTime.hours() << ':' << startTime.minutes() << ' ' << startTime.seconds() << "s\n";

    rsl::time_point currentTime = timer.current_point();
    rsl::time_span elapsedTime = currentTime - startTime;
    rsl::time_span otherElapsed = timer.elapsed_time();
    rsl::time_span accurateElapsed = accurateTimer.elapsed_time();

    std::cout << "timer after log: " << currentTime.hours() << ':' << currentTime.minutes() << ' ' << currentTime.seconds() << "s\n";


    std::cout << "elapsed time: " << elapsedTime.nanoseconds() << " nanoseconds, other elapsed: " << otherElapsed.nanoseconds() << " nanoseconds, accurate elapsed: " << accurateElapsed.nanoseconds() << " nanoseconds\n";


    rsl::buffered_string<16> str = "Hello there!";
    rsl::buffered_string<16> str2 = "This is more than 16 chars!";

    constexpr auto typeName = rsl::type_name(str);

    std::cout << typeName << '\n';

    std::cout << '\"' << str << "\"\n\"" << str2 << "\"\n";

    str.resize(15);

    std::cout << '\"' << str << "\"\n\"" << str2 << "\"\n";

    str.resize(6);

    std::cout << '\"' << str << "\"\n\"" << str2 << "\"\n";

    return 0;
}
