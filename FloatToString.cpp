//
//
//  FloatToString.Cpp
//
//  Created by Ashok K Yannam on 3/4/18.
//  Copyright © 2018 dev-box. All rights reserved.
//

#include <iostream>
#include <math.h>
using namespace std;

class CFloatConverter
{
    
public:
    
    CFloatConverter(char* buffer, uint8_t length, uint8_t precision)
    {
        m_buffer = buffer;
        m_length = length;
        m_precision = precision;
        Reset();
    }
    ~CFloatConverter()
    {
        // Best practice to reset and cleanup any memory this class owns
        m_buffer = 0;
        m_length = 0;
        m_precision = 0;
        Reset();
    }
    
    bool FloatToString(double number);
    void UpdatePrecision(uint8_t precision);
    
private:
    
    bool IntToString(long value, bool considerPrecision);
    bool ReverseString(char* str, uint8_t len);
    void Reset();
    
    char* m_buffer;
    uint8_t m_length;
    uint8_t m_precision;
    uint8_t m_bufferPos;
    
};

void CFloatConverter::UpdatePrecision(uint8_t precision)
{
    m_precision = precision;
}

void CFloatConverter::Reset()
{
    m_bufferPos = 0;
    memset(m_buffer, 0, m_length);
}

bool CFloatConverter::ReverseString(char* str, uint8_t len)
{
    if((str == NULL) || (len == 0))
        return false;
    
    uint8_t index = 0;
    uint8_t rIndex = len - 1;
    char temp;
    
    while(index < len/2)
    {
        temp = str[rIndex];
        str[rIndex] = str[index];
        str[index] = temp;
        index++;
        rIndex--;
    }
    
    return true;
}

bool CFloatConverter::IntToString(long wholeValue, bool considerPrecision)
{
    uint8_t valueLen = 0;
    uint8_t pres = 0;
    
    // Get each digit from right by modulus operation and convert it  to character
    while(wholeValue && (m_length - m_bufferPos -1))
    {
        m_buffer[m_bufferPos++] = wholeValue%10 + '0';
        wholeValue = wholeValue/10;
        valueLen++;
    }
    
    // Consider Precision only for decimal part
    if(considerPrecision)
        pres = m_precision;
    
    // If we ran out of memory, we are done here and return as fail
    if(((wholeValue > 0) && ((m_length - m_bufferPos) <= 1)))
    {
        m_buffer[m_bufferPos] = '\0';
        return false;
    }
    
    // We need to add trailing zeros if the precision is high than the decimal length
    while((valueLen < m_precision) && considerPrecision)
    {
        m_buffer[m_bufferPos++] = '0';
        valueLen++;
    }
    
    // We got all digits in reverse order, now its time to reverse the string to get actual string
    ReverseString(&m_buffer[m_bufferPos - valueLen], valueLen);
    m_buffer[m_bufferPos] = '\0';
    
    return true;
}

bool CFloatConverter::FloatToString(double floatNumber)
{
    if((m_buffer == NULL) || (m_length < 2))
    {
        cout<< "Not Enough Memory" << endl;
        return false;
    }
    
    float rounding = 5/pow(10, m_precision + 1);
    
    // Reset buffer and certain member variables to start fresh conversion
    Reset();
    
    // Deal with negative numbers
    if(floatNumber < 0.0f)
    {
        floatNumber = -1*floatNumber;
        m_buffer[m_bufferPos++] = '-';
    }
    
    // Get the whole integer and decimal portions of the float
    long intPortion = (long)floatNumber;
    double fltPortion = floatNumber - intPortion + rounding;
    
    // If whole integer part if zero, then make sure we consider add character zero
    if(intPortion == 0)
    {
        m_buffer[m_bufferPos++] = '0';
        m_buffer[m_bufferPos]   = '\0';
    }
    else // Convert whole integer part to string
    {
        if(!IntToString(intPortion, false))
        {
            cout<< m_buffer << " FAILED" << endl;
            
            return false;
        }
    }
    
    // Convert decimal part to string
    if(m_precision > 0)
    {
        m_buffer[m_bufferPos++] = '.' ;
        fltPortion = fltPortion*pow(10, m_precision);
        
        if(!IntToString((int)fltPortion, true))
        {
            cout<< m_buffer << " FAILED" << endl;
            return false;
        }
    }
    
    cout<< m_buffer << " SUCCESS" << endl;
    return true;
}

int main(int argc, const char * argv[]) {
    
    static const uint8_t BUFFER_LEN = 100;
    char buffer[BUFFER_LEN];
    double incomingStream[10] = {1.05,-1.06, 156746325457889.2525,1.04, 0.01, 40.05, -156746325.2525, -0.007, -0.006, 0.0};
    CFloatConverter objFltConverter(buffer, BUFFER_LEN, 6);
    
    cout << "****** Precision is at 6 ****" << endl << endl;
    
    for(uint8_t i = 0; i < 10; i++)
    {
        // convert incoming floats values to strings
        objFltConverter.FloatToString(incomingStream[i]);
        
        // verify output with standard library function to_string (precision is defaulted to 6)
        cout << to_string(incomingStream[i]) <<  " [to_string output]" << endl << endl;
        
        if(i == 5)
        {
            cout << "****** Precision changed to 2 ****" << endl << endl;
            objFltConverter.UpdatePrecision(2);
        }
    }
    
    return 0;
}
