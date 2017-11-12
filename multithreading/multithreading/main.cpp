#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include <fstream>

using namespace std;

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

int main()
{
	LofFile log;
	thread t1(function_1, std::ref(log));
	
	for (int i = 0; i < 100; ++i)
	{
		log.shared_print("from main: ", i);
	}
	t1.join();
}