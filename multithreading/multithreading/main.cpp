#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include <fstream>
#include <future>
#include <condition_variable>

using namespace std;
#if 0
mutex mu;

#if 0
class LofFile{
public:
	LofFile()
	{
		f.open("log.txt");
	}
	// 
	void shared_print(string id, int value)
	{
		std::lock(m_mutex, m_mutex2);

		std::lock_guard<mutex> locker(m_mutex, std::adopt_lock);
		std::lock_guard<mutex> locker2(m_mutex2, std::adopt_lock);
		//f << "From " << id << ": " << value << endl;
		cout << __FUNCTION__ << " From " << id << ": " << value << endl;
	}

	void shared_print2(string id, int value)
	{
		std::lock(m_mutex, m_mutex2);

		std::lock_guard<mutex> locker2(m_mutex2, std::adopt_lock);
		std::lock_guard<mutex> locker(m_mutex, std::adopt_lock);
		
		//f << "From " << id << ": " << value << endl;
		cout << __FUNCTION__ << " From " << id << ": " << value << endl;
	}
private:
	mutex m_mutex;
	mutex m_mutex2;
	ofstream f;
};
#elif 1
class LofFile{
public:
	LofFile()
	{
		//f.open("log.txt");
	}
	// 
	void shared_print(string id, int value)
	{
		//{
		//	std::unique_lock<mutex> locker(m_mutex_open, std::defer_lock);
		//	if (!f.is_open())
		//	{
		//		f.open("log.txt");
		//	}
		//}
		//

		std::call_once(m_flag, [&](){f.open("log.txt");});

		std::unique_lock<mutex> locker(m_mutex, std::defer_lock);

		//...
		locker.lock();
		//f << "From " << id << ": " << value << endl;
		cout << __FUNCTION__ << " From " << id << ": " << value << endl;
		locker.unlock();

		locker.lock();

		std::unique_lock<mutex> locker2 = std::move(locker);
	}

	
private:
	mutex m_mutex;
	//mutex m_mutex_open;
	std::once_flag m_flag;
	ofstream f;
};
#endif


void shared_print(string msg, int id)
{
	/*mu.lock();
	cout << msg << id << endl;
	mu.unlock();*/

	/*std::lock_guard<mutex> guard(mu);
	cout << msg << id << endl;*/
}

void function_1(LofFile& log)
{
	for (int i = 0; i > -100; --i)
	{
		log.shared_print("from t1: ", i);
	}
}


class Fctor{
public:
	void operator()(string& msg) {
		/*for (int i = 0; i > -100; --i)
		{
			cout << "from t1: [" << i << "]"<< msg << endl;
		}*/
		cout << __FUNCTION__ << ':' << this_thread::get_id() << endl;
		msg = "I Love Liuxiangyan";
	}
};
int main0()
{
	/*Fctor fct;*/
	string str = "I love C++";
	cout << __FUNCTION__ << ':' << this_thread::get_id() << endl;
	//thread t1(/*fct*/(Fctor()), std::ref(str));
	thread t1(/*fct*/(Fctor()), std::move(str));
	/*try{
		for (int i = 0; i < 100; ++i)
		{
			cout << "from main: " << i << endl;
		}
	}
	catch (...){
		t1.join();

		throw;
	}*/
	thread t2 = std::move(t1);
	t2.join();
	cout << "from main: " << str << endl;
	cout << "concurrency() = "<< thread::hardware_concurrency() << endl;
	return 0;
}

int main1()
{
	LofFile log;
	thread t1(function_1, std::ref(log));
	
	for (int i = 0; i < 100; ++i)
	{
		log.shared_print("from main: ", i);
	}
	t1.join();
}
#endif
////////////////////////////////////////////////////////////

#if 0


std::queue<int> q;
std::mutex mu;
std::condition_variable cond;

void function_1()
{
	int count = 10;
	while (count > 0){
		std::unique_lock<mutex> locker(mu);
		q.push(count);
		locker.unlock();
		//cond.notify_one();
		cond.notify_all();
		std::this_thread::sleep_for(chrono::seconds(1));
		count--;
	}
}

void function_2()
{
	int data = 0;
	while (data != 1)
	{
		std::unique_lock<mutex> locker(mu);
		cond.wait(locker, [](){return !q.empty();});  //线程会休眠

		data = q.back();
		q.pop();
		locker.unlock();
		cout << "t2 got a value from queue : " << data << endl;
	}
}

int main(void)
{
	thread t1(function_1);
	thread t2(function_2);

	t1.join();
	t2.join();
	return 0;
}
#endif


#if 0


int factorial(std::shared_future<int> f)
{
	int res = 1;
	int N = f.get(); // throw std::future_errc::broken_promise
	for (int i = N; i > 1; --i)
	{
		res *= i;
	}
	//cout << "Result is " << res << endl;
	//x = res;
	return res;
}

int main(void)
{
	int x = 0;
	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::shared_future<int> sf = f.share();

	/*std::thread t1(factorial, 4, std::ref(x));
	t1.join();*/
	//std::future<int> fu = std::async(std::launch::deferred, factorial, 4); // not use thread
	//std::future<int> fu = std::async(factorial, 4);
	std::future<int> fu = std::async(std::launch::deferred | std::launch::async, factorial, sf); //默认的
	std::future<int> fu1 = std::async(std::launch::deferred | std::launch::async, factorial, sf); //默认的
	std::future<int> fu2 = std::async(std::launch::deferred | std::launch::async, factorial, sf); //默认的
	std::future<int> fu3 = std::async(std::launch::deferred | std::launch::async, factorial, sf); //默认的
	std::future<int> fu4 = std::async(std::launch::deferred | std::launch::async, factorial, sf); //默认的

	p.set_value(4);
	x = fu.get();  // only use once
	cout << "main : x = " << x << endl;
	return 0;
}
#endif

#if 0


class A
{
public:
	void f(int x, char c){}
	int operator()(int N){ return 0; }
};

void foo(int x){}

int main()
{
	A a;
	std::thread t1(a, 6);			//传递a的拷贝给子线程
	std::thread t2(std::ref(a), 6);	//传递a的引用给子线程
	std::thread t3(std::move(a), 6);//a在主线程中不再有效
	std::thread t4(A(), 6);			//传递临时创建的a对象给子线程

	std::thread t5(foo, 6);
	std::thread t6([](int x){return x*x; }, 6);
	std::thread t7(&A::f, a, 8, 'w');		//传递a的拷贝的成员函数给子线程
	std::thread t8(&A::f, &a, 8, 'w');		//传递a的地址的成员函数给子线程

	std::async(std::launch::async, a, 6);		//同样可以使用
	return 0;
}
#endif 


#if 0


std::queue<std::packaged_task<int()> > task_q;
std::mutex mu;
std::condition_variable cond;

int factorial(int N)
{
	int res = 1;
	for (int i = N; i > 1; --i)
	{
		res *= i;
	}
	cout << "Result is " << res << endl;
	//x = res;
	return res;
}

void thread_1()
{
	std::packaged_task<int()> t;
	{
		std::unique_lock<mutex> locker(mu);
		cond.wait(locker, [](){return !task_q.empty();});
		t = std::move(task_q.front());
	}
	t();
}

int main()
{
	//auto tb = std::bind(factorial, 5);
	//tb();
	std::thread t1(thread_1);
	std::packaged_task<int()> t(std::bind(factorial, 4));
	std::future<int> ret = t.get_future();
	{
		std::unique_lock<mutex> locker(mu);
		//task_q.push(std::move(t));
		task_q.push(std::move(t));
		cond.notify_all();
	}
	
	int value = ret.get();
	cout << "value = " << value << endl;

	t1.join();
	
	return 0;
}
#endif


int factorial(int N)
{
	int res = 1;
	for (int i = N; i > 1; --i)
	{
		res *= i;
	}
	cout << "Result is " << res << endl;
	//x = res;
	return res;
}

int main(void)
{
	std::thread t1(factorial, 6);
	std::this_thread::sleep_for(chrono::milliseconds(3));
	chrono::steady_clock::time_point tp = chrono::steady_clock::now() + chrono::milliseconds(4);
	std::this_thread::sleep_until(tp);

	std::timed_mutex tmu;
	std::unique_lock<timed_mutex> tlocker(tmu);
	tlocker.try_lock();
	tlocker.try_lock_for(chrono::milliseconds(3));
	tlocker.try_lock_until(tp);

	std::mutex mu;
	std::unique_lock<mutex> locker(mu);
	std::condition_variable cond;
	cond.wait_for(locker, chrono::milliseconds(3));
	cond.wait_until(locker, tp);

	std::promise<int> p;
	std::future<int> f = p.get_future();
	f.wait_for(chrono::milliseconds(3));
	f.wait_until(tp);
	return 0;
}