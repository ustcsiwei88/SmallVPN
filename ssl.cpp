#include <iostream>    
#include <cassert>  
#include <string>  
#include <cstring>  
#include <vector>       
#include <openssl/rsa.h>    
#include <openssl/pem.h>    

  
void generateRSAKey(std::string strKey[2])  
{        
    strKey[0] = "-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEAv9AV95EP5gIDDfaiyXfMwPDOrGtwVHkvuT1GuAZKnbNWa0kdNZeN\n60ZBFBALOwVVRKmnAb6vsZJiOpA8FlAVNGTZSrQunw9WVaVGzQhk2UvxMqocVJ8D\ny8JBJii5lgzccXIlyvakkhw4eHAEzOkNYFEOMx+jCniP8ZYsbtjvbBak2pzVfh5H\nC6JM0yWvh6fWF5WACgYcO1iW39h9EZsps2lewhsP/AonYuUAIX/Jkf5DJ7A2z2YF\n5zyOlJnqS6fB81rN6rUUM5ENq5y2scAYJmgi90GrOtEuRM0q3UGuf7V/ZB5egnIP\nrLE4a0dYlk1OmBstoaXcEZBhbmI/ANj3xwIBAw==\n-----END RSA PUBLIC KEY-----";  
    strKey[1] = "-----BEGIN RSA PRIVATE KEY-----\nMIIEogIBAAKCAQEAv9AV95EP5gIDDfaiyXfMwPDOrGtwVHkvuT1GuAZKnbNWa0kd\nNZeN60ZBFBALOwVVRKmnAb6vsZJiOpA8FlAVNGTZSrQunw9WVaVGzQhk2UvxMqoc\nVJ8Dy8JBJii5lgzccXIlyvakkhw4eHAEzOkNYFEOMx+jCniP8ZYsbtjvbBak2pzV\nfh5HC6JM0yWvh6fWF5WACgYcO1iW39h9EZsps2lewhsP/AonYuUAIX/Jkf5DJ7A2\nz2YF5zyOlJnqS6fB81rN6rUUM5ENq5y2scAYJmgi90GrOtEuRM0q3UGuf7V/ZB5e\ngnIPrLE4a0dYlk1OmBstoaXcEZBhbmI/ANj3xwIBAwKCAQB/4A6lC1/urAIJTxcw\n+oiAoInIR6A4UMp7fi8lWYcTzORHhhN5D7PyLtYNYAd8rjjYcRoBKcp2YZbRtX1k\nNWN4QzuHIsm/X47jw4SIsEM7h/Yhxr2Nv1fdLCtuxdEOsz2g9sPcpG22vXr69ViI\nm15ANgl3amyxpbVLuXL0kJ+dY0Y/Q/zKaOTM7AhxUJcioDp6Y7/zOWIlKIQCY0Gq\nLJJkkCMY4JK6ksWx/pPQvwJBmwbnN0i+OsxEqXh0aTCEaTFzAeX5gw/DxXsE1lsc\nJAZRObC1rGmlk7xW9jLmzxzbBY+zMgUix7wCVS+2FgiAUh4gwT0qzRrucmcqq0PT\nccPrAoGBAOXR/SKZ8hgBAUFcuB/iREH2ZzcE+Kru2wmRhJDuDmTEi1HLUQ5SvBuB\nUrXKlDPvHUDHRXnbUsMfJA5mjxbuIwAZl22+EfnJyreYqBfWEMR1wV3vy7r/RHCK\nePILJB4CJJKKSTxM9jqPCmnTRRBY5Wc5OKTgT68ye0dUjyVwORsXAoGBANWpube0\njtfXPv7MdKzxczxpmKkYN14UwMdKvwmkSelYBdhReSWRceNWlGydzRJAeHgFl8/W\nuxBhGkt/bQqW5vetQIc2XoLEHh2LsxYxeUo20Dv281HDLyM34ux35uU1CJVNChqB\nUDsaJwcG4Tt87LotwK0BIoR5aX9Z0jPTdTbRAoGBAJk2qMG79rqrViuTJWqW2Cv5\nmiSt+xyfPLELrbX0CZiDB4vc4LQ3KBJWNyPcYs1KE4CE2PvnjIIUwrREX2SewgAR\nD55+tqaGhyUQcA/kCy2j1j6f3SdU2EsG+0wHbWlWwwxcMNLd+XxfXEaM2LWQmO97\nexiVinTMUi+NtMOgJhIPAoGBAI5xJnp4XzqPf1SITcig932buxtlej64gITcf1vC\n2/DlWTrg+25g9pePDZ2+iLbVpaVZD9/kfLWWEYeqSLG570/I1a95lFctaWkHzLl2\nUNwkitKkojaCH2zP7J2lRJjOBbjeBrxWNXy8GgSvQNJTSHwegHNWFwL7m6o74Xfi\nTiSLAoGAM0e2+VKN/2nNPLluFCaVYfJ/dup4N4kF9ZKdj+iXPkzMIJypEA9sUt1q\ndUEVUfZIXYH/WuslYkoUIsi7LNYIniBdWOocKqBnKOvJiW63p32K1+omDnDQeOAw\nHF0IvZqWhyQ487/ZAjz0SNHIkmrylJwTUVJ32f30HlOzL4e3QGU=\n-----END RSA PRIVATE KEY-----";  
   
}  
    
std::string rsa_pub_encrypt(const std::string &clearText, const std::string &pubKey)  
{  
    std::string strRet;  
    RSA *rsa = NULL;  
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);   
    RSA* pRSAPublicKey = RSA_new();  
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);  
  
    int len = RSA_size(rsa);  
    char *encryptedText = (char *)malloc(len + 1);  
    memset(encryptedText, 0, len + 1);  
  
    // 加密函数  
    int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
        strRet = std::string(encryptedText, ret);  
  
    // 释放内存  
    free(encryptedText);  
    BIO_free_all(keybio);  
    RSA_free(rsa);  
  
    return strRet;  
}  
  
std::string rsa_pri_decrypt(const std::string &cipherText, const std::string &priKey)  
{  
    std::string strRet;  
    RSA *rsa = RSA_new();  
    BIO *keybio;  
    keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);  
  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);  
  
    int len = RSA_size(rsa);  
    char *decryptedText = (char *)malloc(len + 1);  
    memset(decryptedText, 0, len + 1);  
  
    // 解密函数  
    int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
        strRet = std::string(decryptedText, ret);  
  
    // 释放内存  
    free(decryptedText);  
    BIO_free_all(keybio);  
    RSA_free(rsa);  
  
    return strRet;  
}  
  
int main(int argc, char **argv)  
{  
    std::string srcText = "this is an example";  
  
    std::string encryptText;  
    std::string encryptHexText;  
    std::string decryptText;  
  
    std::cout << "=== text ===" << std::endl;  
    std::cout << srcText << std::endl;  
    // rsa    
    std::cout << "=== rsa cryptation ===" << std::endl;  
    std::string key[2];  
    generateRSAKey(key);  
    std::cout << "pub key: " << std::endl;  
    std::cout << key[0] << std::endl;  
    std::cout << "pri key: " << std::endl;  
    std::cout << key[1] << std::endl;  
    encryptText = rsa_pub_encrypt(srcText, key[0]);  
    std::cout << "encrypted text： " << std::endl;  
    std::cout << encryptText << std::endl;  
    decryptText = rsa_pri_decrypt(encryptText, key[1]);  
    std::cout << "decrypted text: " << std::endl;  
    std::cout << decryptText << std::endl;  
    return 0;  
}