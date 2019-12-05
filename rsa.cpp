#include <gmpxx.h>
#include <bits/stdc++.h>
#include<time.h>
#include<fstream>
#include<string.h>
#include<iostream>
#include <cstdio>
#include <ctime>
#include <string>
#include <stack>
#include <cstring>
#include <cstdlib>
using namespace std;

mpz_t key_p, key_q, key_n, key_f, key_e, key_d, M, C, M2;


void generate(int keysize){
	gmp_randstate_t grt;
	gmp_randinit_default(grt);
    //set random seed as current time
	gmp_randseed_ui(grt, time(NULL)); 
    //init
	mpz_init(key_p); 
	mpz_init(key_q);
 
	mpz_urandomb(key_p, grt, keysize);
	mpz_urandomb(key_q, grt, keysize);
    //generate primes p,q
	mpz_nextprime(key_p, key_p);  
	mpz_nextprime(key_q, key_q);
    // n = p*q
	mpz_init(key_n);
	mpz_mul(key_n, key_p, key_q);
 
	//f = (p-1)*(q-1)
	mpz_init(key_f);
	mpz_sub_ui(key_p, key_p, 1);
	mpz_sub_ui(key_q, key_q, 1);
	mpz_mul(key_f, key_p, key_q);
 
	//let e = 65537, the public key is gcd(e,n)
	mpz_init_set_ui(key_e, 65537);
    // write pub key to file ...
    
    //private key is the module inverse of e mod f
    mpz_init(key_d);
	mpz_invert(key_d, key_e, key_f);
    // write pri key to file ...

}

void encrtpt(string str){
    string M_str = "";
    for (int i = 0; i < str.length(); i++)
    {
        char x = str.at(i);
        int num = int(x);
		if(num>=100){
			M_str += to_string(num);
		}
		else{
        	M_str = M_str + "0" + to_string(num);
		}
    }
    // cout<<M_str.c_str()<<endl;
    mpz_init_set_str(M, M_str.c_str(),16);
	mpz_init(C);
    //C=M^e mod n;
	mpz_powm(C, M, key_e, key_n);  
}

stack<char> decrtpt(){
	//mpz_init_set_str(C2,M2,16);
	mpz_init(M2);
	//M=C^d mod n;
	mpz_powm(M2, C, key_d, key_n);
	char encrtpted_char[200];
	mpz_get_str(encrtpted_char, 16, M2);
	string encrtpted_str = encrtpted_char;
	int en_len = encrtpted_str.length();
	stack<char> s;
	for (int k = 0; k < en_len; k++){
		if (k % 3 ==0 && k > 0){
			string tmp_str = encrtpted_str.substr(en_len-k,3);
			if (tmp_str.find("0") == 0){
				s.push(char(stoi(tmp_str.substr(1,2))));
				// cout<<char(stoi(tmp_str.substr(1,2)))<<endl;
			}
			else{
				s.push(char(stoi(tmp_str)));
				// cout<<char(stoi(tmp_str))<<endl;
			}
			if (en_len-k<=2){
				s.push(char(stoi(encrtpted_str.substr(0,en_len-k))));
				// cout<<char(stoi(encrtpted_str.substr(0,en_len-k)))<<endl;
			}
		}
	}
	return s;
}

void destroy(){
    mpz_clear(key_q);
	mpz_clear(key_p);
	mpz_clear(M);
	mpz_clear(C);
	mpz_clear(M2);
	mpz_clear(key_n);
	mpz_clear(key_f);
	mpz_clear(key_d);
}

int main()
{
    string str = "asdvhv,as123274o2ryhujsbcgug123qwugdb2v1eg1271guhbad";
	cout<<"the plaintext is "<< str << endl;
	generate(1024);
	encrtpt(str);
	gmp_printf("%s %ZX\n", "the cipertxt is", C);
	stack<char> s;
	s = decrtpt();
	while (!s.empty()) 
    { 
        cout << " "<< s.top(); 
        s.pop(); 
    }
	cout<<" "<<endl;
	destroy();
    return 0;
}
