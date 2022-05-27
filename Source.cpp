#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <complex>
#include <fstream>
#include <iostream>
#include <amp.h>
#include <amp_math.h>
#include <chrono>

// Imports
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::complex;
using std::cout;
using std::endl;
using std::ofstream;
using std::fstream;
using std::chrono::microseconds;
using namespace std;
using namespace concurrency;

//Define "the_clock"
typedef std::chrono::steady_clock the_clock;

//The size of the hash
const int HASHNUM = 3000;
const int USERHASHNUM = 50;
const int TS1 = 30;
const int TS2 = 32;
const int ITERATIONS = 1000;

//Init values
#define INIT_A 0x67452301
#define INIT_B 0xefcdab89
#define INIT_C 0x98badcfe
#define INIT_D 0x10325476

#define SQRT_2 0x5a827999
#define SQRT_3 0x6ed9eba1

unsigned int nt_buffer[16];
unsigned int output[4];
char hex_format[33];
char itoa16[17] = "0123456789ABCDEF";

string NTLM(char* key)
{
	/*
	------------------------------------------------------------------------------------------------------------------------------------------------
	| The code in this function is not mine, all credit goes to [Mustafa Chelik] - https://www.codeproject.com/Articles/328761/NTLM-Hash-Generator |
	------------------------------------------------------------------------------------------------------------------------------------------------

	Some minor alterations have been made but otherwise this function is entirely his work

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 Prepare the string for hash calculation
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	*/
	int i = 0;
	int length = strlen(key);
	memset(nt_buffer, 0, 16 * 4);
	//The length of key need to be <= 27
	for (; i < length / 2; i++)
		nt_buffer[i] = key[2 * i] | (key[2 * i + 1] << 16);

	//padding
	if (length % 2 == 1)
		nt_buffer[i] = key[length - 1] | 0x800000;
	else
		nt_buffer[i] = 0x80;
	//put the length
	nt_buffer[14] = length << 4;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// NTLM hash calculation
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	unsigned int a = INIT_A;
	unsigned int b = INIT_B;
	unsigned int c = INIT_C;
	unsigned int d = INIT_D;

	/* Round 1 */
	a += (d ^ (b & (c ^ d))) + nt_buffer[0]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[1]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[2]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[3]; b = (b << 19) | (b >> 13);

	a += (d ^ (b & (c ^ d))) + nt_buffer[4]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[5]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[6]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[7]; b = (b << 19) | (b >> 13);

	a += (d ^ (b & (c ^ d))) + nt_buffer[8]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[9]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[10]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[11]; b = (b << 19) | (b >> 13);

	a += (d ^ (b & (c ^ d))) + nt_buffer[12]; a = (a << 3) | (a >> 29);
	d += (c ^ (a & (b ^ c))) + nt_buffer[13]; d = (d << 7) | (d >> 25);
	c += (b ^ (d & (a ^ b))) + nt_buffer[14]; c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a))) + nt_buffer[15]; b = (b << 19) | (b >> 13);

	/* Round 2 */
	a += ((b & (c | d)) | (c & d)) + nt_buffer[0] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[4] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[8] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[12] + SQRT_2; b = (b << 13) | (b >> 19);

	a += ((b & (c | d)) | (c & d)) + nt_buffer[1] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[5] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[9] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[13] + SQRT_2; b = (b << 13) | (b >> 19);

	a += ((b & (c | d)) | (c & d)) + nt_buffer[2] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[6] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[10] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[14] + SQRT_2; b = (b << 13) | (b >> 19);

	a += ((b & (c | d)) | (c & d)) + nt_buffer[3] + SQRT_2; a = (a << 3) | (a >> 29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[7] + SQRT_2; d = (d << 5) | (d >> 27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[11] + SQRT_2; c = (c << 9) | (c >> 23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[15] + SQRT_2; b = (b << 13) | (b >> 19);

	/* Round 3 */
	a += (d ^ c ^ b) + nt_buffer[0] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[8] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[4] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[12] + SQRT_3; b = (b << 15) | (b >> 17);

	a += (d ^ c ^ b) + nt_buffer[2] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[10] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[6] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[14] + SQRT_3; b = (b << 15) | (b >> 17);

	a += (d ^ c ^ b) + nt_buffer[1] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[9] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[5] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[13] + SQRT_3; b = (b << 15) | (b >> 17);

	a += (d ^ c ^ b) + nt_buffer[3] + SQRT_3; a = (a << 3) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[11] + SQRT_3; d = (d << 9) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[7] + SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[15] + SQRT_3; b = (b << 15) | (b >> 17);

	output[0] = a + INIT_A;
	output[1] = b + INIT_B;
	output[2] = c + INIT_C;
	output[3] = d + INIT_D;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Convert the hash to hex (for being readable)
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for (i = 0; i < 4; i++)
	{
		int j = 0;
		unsigned int n = output[i];
		//iterate the bytes of the integer		
		for (; j < 4; j++)
		{
			unsigned int convert = n % 256;
			hex_format[i * 8 + j * 2 + 1] = itoa16[convert % 16];
			convert = convert / 16;
			hex_format[i * 8 + j * 2 + 0] = itoa16[convert % 16];
			n = n / 256;
		}
	}

	string hash = hex_format; //hex_format contains the NTLM hash

	for_each(hash.begin(), hash.end(), [](char& x)
		{
			x = ::toupper(x);
		}
	);
	return hash;
}

void compute_hash()
{
	string str;
	string str2;

	ifstream file("rockyouSmall.txt"); //Loads file containing common passwords

	ifstream file2("UserHash.txt"); //Loads user hashes


	int iteration = 0;

	static string hashes[HASHNUM];
	static string passwords[HASHNUM];

	while (getline(file, str)) //Iterates the dictionary text file
	{
		char* char_arr; //Pointer to char

		char_arr = &str[0]; //sets pointer to string so it works in params later

		passwords[iteration] = char_arr;

		hashes[iteration] = NTLM(char_arr); //Converts string to NTLM

		iteration++;
	}

	iteration = 0;

	static string UserHashes[USERHASHNUM];

	while (getline(file2, str2)) //Iterates the password dictionary line by line
	{
		UserHashes[iteration] = str2; //Fills array
		iteration++;
	}

	//initialises some values
	uint32_t UserHash[32] = { 0 };
	uint32_t Dict[HASHNUM][32] = { 0 };

	uint32_t Output[HASHNUM][32] = { 0 };

	uint32_t Info[1] = { 0 };

	//fills array
	for (int j = 0; j < HASHNUM; j++)
	{
		for (unsigned i = 0; i < hashes[j].size(); ++i)
		{
			Dict[j][i] = uint32_t(hashes[j][i]);
		}
	}

	//Declares array views and some variables
	uint32_t* pUserHashes = &(UserHash[0]);
	uint32_t* pDict = &(Dict[0][0]);

	uint32_t* pOutput = &(Output[0][0]);
	uint32_t* pInfo = &(Info[0]);

	const array_view<uint32_t, 2> a(HASHNUM, 32, pDict);
	array_view<uint32_t, 1> b(32, pUserHashes);

	array_view<uint32_t, 2> c(HASHNUM, 32, pOutput);
	array_view<uint32_t, 1> d(1, pInfo);

	int cracked = 0;
	int fail = 0;
	int total = 0;
	long time_taken = 0;
	for (int x = 0; x < ITERATIONS; x++)
	{
		the_clock::time_point start = the_clock::now();
		for (int j = 0; j < USERHASHNUM; j++)
		{
			c[0][0];
			d[0] = 0;
			memset(Output, 0, sizeof(Output));

			for (unsigned i = 0; i < UserHashes[j].size(); ++i)
			{
				b[i] = uint32_t(UserHashes[j][i]);
			}
			// Checks for matches on ASCII values in parallel
			parallel_for_each(a.extent.tile<TS1, TS2>(), [=](concurrency::tiled_index<TS1, TS2> t_idx) restrict(amp)
				{
					int Hash = t_idx.global[0];
					int CharNum = t_idx.global[1];
					int HashLocal = t_idx.local[0];
					int CharNumLocal = t_idx.local[1];

					tile_static int t[TS1][TS2];

					t[HashLocal][CharNumLocal] = a[t_idx];

					int val = 0;

					if (t[HashLocal][CharNumLocal] == b[CharNum])
					{
						c[Hash][CharNum] = 1;
					}

					t_idx.barrier.wait();
					//Barrier makes sure that the variables have been properly flagged before it checks them

					if (CharNum == 1)
					{
						for (int i = 0; i < 32; i++)
						{
							if (c[Hash][i] == 1)
							{
								val++;
							}
							if (val == 31)
							{
								d[0] = Hash;
							}
						}
					}
				});

			if (d[0] != 0)
			{
				//cout << j + 1 << ".) " << hashes[d[0]] << " = " << passwords[d[0]] << endl << endl;
				cracked++;
			}
			else
			{
				//cout << j + 1 << ".) No match" << endl << endl;
				fail++;
			}
		}

		the_clock::time_point end = the_clock::now();
		time_taken = duration_cast<microseconds>(end - start).count();

		//cout << "Decrypter cracked " << cracked << "/" << cracked + fail << " in " << time_taken << " Micro Seconds." << endl;

		cracked = 0;
		fail = 0;
		total = total + time_taken;
	}
	cout << "Average time taken is " << total / ITERATIONS << " Micro Seconds." << endl;
	cin.get();
}


int main(int argc, char* argv[])
{
	compute_hash();
	return 0;
}