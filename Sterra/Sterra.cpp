#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <ctime>

//функция bit_count занимается подсчетом либо единичных либо нулевых битов в зависимости от параметра value
template <typename T>
int bit_count(T input, bool value);
//функция delete_elem занимается удалением элемента либо из начала либо из конца списка в зависимости от параметра value
void delete_elem(std::list<int>&l, bool value);
//функция generate_list заполняет список рандомными значениями
void generate_list(std::list<int>&l);
//функция print_list выводит значения элементов списка в консоль
void print_list(std::list<int>&l);
//функция work_with_list выполняет основную логику программы по работе со списком
void work_with_list(std::list<int>&l, bool value, int &counter_of_bits, int &counter_Of_delete_elems);
std::mutex mtx;

int main()
{
	//srand(static_cast<unsigned int>(time(0)));
	setlocale(LC_ALL, "ru");
	//размер списка
	int size = 1100;

	int counter_of_zero_bits = 0;
	int counter_of_non_zero_bits = 0;

	int counter_of_delete_elem_1 = 0;
	int counter_of_delete_elem_2 = 0;

	//генерация списка
	std::list<int> l(size);
	generate_list(l);
	print_list(l);
	std::cout << "\n";

	std::thread th1(work_with_list, std::ref(l), false, std::ref(counter_of_zero_bits), std::ref(counter_of_delete_elem_1));
	std::thread th2(work_with_list, std::ref(l), true, std::ref(counter_of_non_zero_bits), std::ref(counter_of_delete_elem_2));
	th1.join();
	th2.join();

	std::cout << "Первый поток учел " << counter_of_zero_bits << " нулевых бит и удалил " << counter_of_delete_elem_1 << " элементов\n";
	std::cout << "Второй поток учел " << counter_of_non_zero_bits << " единичных бит и удалил " << counter_of_delete_elem_2 << " элементов\n";

	return 0;
}

template <typename T>
int bit_count(T input, bool value) {
	int res = 0;
	static const unsigned shift_size = ((sizeof(T) * 8) - 1);
	if (input < 0) {
		res++;
		input &= ((1 << shift_size) - 1);
	}
	while (input) {
		res += (input & 1);
		input >>= 1;
	}
	switch (value)
	{
	case false:
		return (sizeof(T) * 8) - res;
	case true:
		return res;
	}
}
void delete_elem(std::list<int>&l, bool value)
{
	switch (value)
	{
	case true:
		l.pop_back();
		break;
	case false:
		l.pop_front();
		break;
	}
}

void generate_list(std::list<int>&l)
{
	std::list<int>::iterator it{ l.begin() };
	for (auto i = it; i != l.end(); ++i)
	{
		*i = rand();
	}
}
void print_list(std::list<int>&l)
{
	std::list<int>::iterator it{ l.begin() };
	for (auto i = it; i != l.end(); ++i)
	{
		std::cout << *i << " ";
	}
}
void work_with_list(std::list<int>&l, bool value, int &counter_of_bits, int &counter_Of_delete_elems)
{
	while (true)
	{
		mtx.lock();
		if (l.size() > 0)
		{
			switch (value)
			{
			case false:
				counter_of_bits += bit_count(*(l.begin()), value);
				delete_elem(l, value);
				counter_Of_delete_elems++;
				break;

			case true:
				counter_of_bits += bit_count(*(l.rbegin()), value);
				delete_elem(l, value);
				counter_Of_delete_elems++;
				break;
			}
		}
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (l.size() == 0)
			break;
	}
}