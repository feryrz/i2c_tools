#include <Wire.h>

#define EOH         '\n'
#define MIN(X, Y)   (((X) < (Y)) ? (X) : (Y))

uint8_t buf[1024];
uint16_t buf_len = 0;

void i2c_scanner(void);
bool parse_hex(String in, uint8_t *res, uint16_t res_len);
void dump_var(uint8_t *p, uint16_t len);

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    Serial.println("-i2c tools-\r\n");
}

void loop()
{
    String in = Serial.readStringUntil(EOH);
    if (in.length() > 0) {
        in = in.substring(0, in.indexOf('\n') - 1);
        in.replace(" ", "");
        Serial.println("Input (" + String(in.length()) + "): " + in);

        /*
        if (in[0] == 'S') {
            if (parse_hex(in.substring(1), buf, &buf_len, sizeof(buf))) {
                Serial.print("Write: ") dump_var(buf, buf_len); Serial.println();
                Wire.beginTransmission(buf[0]);
                Wire.write(buf[1], buf_len - 1);
                Wire.endTransmission();
            } else {
                Serial.println("Invalid Input");
            }
        }

        if (in[0] == 'R') {
            uint16_t len = 0;
            if (parse_hex(in.substring(1), buf, &buf_len, sizeof(buf))) {
                Serial.printf("Read (%d)\r\n", buf[0]);
            } else {
                Serial.println("Invalid Input");
            }
        }
        */

        if (in[0] == 'R') {
            uint16_t len = 0;

            if (parse_hex(in.substring(1), buf, &buf_len, sizeof(buf))) {
                Serial.printf("Send (%d): ", buf_len); dump_var(buf, buf_len); Serial.println();

                Wire.beginTransmission(buf[0]);
                Wire.write(buf[1]);
                Wire.endTransmission();

                buf_len = Wire.requestFrom(buf[0], buf[2], true);
                Wire.readBytes(buf, buf_len);
                Serial.println("Result: "); dump_var(buf, buf_len); Serial.println();
            } else {
                Serial.println("Input Error.!");
            }
        }
    }
}

bool parse_hex(String in, uint8_t *res, uint16_t *res_len, uint16_t max_res)
{
    in.toUpperCase();
    
    if (in.length() % 2 != 0) {
        Serial.println("Input Error!");
        return false;
    }

    uint16_t i = 0;
    char *str, _buf[3] = { 0, 0, 0 };
    for (i = 0; i < in.length() / 2; i++) {
        in.substring(i * 2, (i * 2) + 2).toCharArray(_buf, 3);

        if (res && i < max_res)
            res[i] = strtol(_buf, &str, HEX);
        else
            break;
    }
    *res_len = i;

    return true;
}

void dump_var(uint8_t *p, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        Serial.printf("%02X ", p[i]);
    }
}

void i2c_scanner(void)
{
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
            Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
       Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
}