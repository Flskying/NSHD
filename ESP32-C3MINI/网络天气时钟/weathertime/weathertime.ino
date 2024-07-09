#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Ticker.h> //定时器
#include <Arduino.h>
#include <U8g2lib.h>

Ticker timer1;//定时器对象1
Ticker timer2;//对象2

//***************修改一*****************************
//这里wifi必须为2.4G！！！   5G网络将无法连接
const char *ssid = "404! Network error";//wifi名称(改成自己的)
const char *password = "ZQH%15034695001.";//wifi密码(改成自己的)
//****************************************************

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//使用i2c驱动，可以修改成自己使用的端口
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int n=0;//判断时间刷新标志
int jj=0;//天气更新标志

int currentYear;
int currentMonth;
int monthDay;
int weekDay;
int currentHour;
int currentMinute;
int currentSecon;
unsigned long myTime;//计时器
int results_0_now_code_int;//天气代码
int results_0_now_temperature_int;//温度

WiFiUDP ntpUDP;

// const char* ssid = "404! Network error";
// const char* password = "ZQH%15034695001.";

const char* ntpServer = "ntp1.aliyun.com";
const long gmtOffset_sec = 8 * 3600;  // GMT+8:00, Beijing time
const int daylightOffset_sec = 0;
int time_ok = 0;
int timenum=1;
int temptest=1;
// //开启ntp时间协议
// NTPClient timeClient(ntpUDP, "ntp1.aliyun.com");

const char* host = "api.seniverse.com";     // 将要连接的服务器地址  
const int httpPort = 80;                    // 将要连接的服务器端口      

// 心知天气HTTP请求所需信息
String reqUserKey = "SguN9Q5nx54zA-Kg4";   // 私钥：心知天气官网注册账号免费使用，将自己的私钥复制
String reqLocation = "TaiYuan";            // 城市，改成你想要获取信息的城市英文
String reqUnit = "c";                      // 摄氏/华氏

// 建立心知天气API当前天气请求资源地址
void get_weather(){
  String reqRes = "/v3/weather/now.json?key=" + reqUserKey +
                  + "&location=" + reqLocation + 
                  "&language=en&unit=" +reqUnit;
  httpRequest(reqRes);
  delay(3000);
}

// 向心知天气服务器服务器请求信息并对信息进行解析
void httpRequest(String reqRes){
  WiFiClient client;

  // 建立http请求信息
  String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" + 
                              "Host: " + host + "\r\n" + 
                              "Connection: close\r\n\r\n";
  // 尝试连接服务器
  if (client.connect(host, 80)){
    Serial.println(" Success!");
 
    // 向服务器发送http请求信息
    client.print(httpRequest);
 
    // 获取并显示服务器响应状态行 
    String status_response = client.readStringUntil('\n');
 
    // 使用find跳过HTTP响应头
    if (client.find("\r\n\r\n")) {
      // Serial.println("Found Header End. Start Parsing.");
    }
    
    // 利用ArduinoJson库解析心知天气响应信息
    parseInfo(client); 
  } else {
    Serial.println(" connection failed!");
  }   
  //断开客户端与服务器连接工作
  client.stop(); 
}

//心知天气对应数值提取函数
void parseInfo(WiFiClient client){
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 230;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, client);
  JsonObject results_0 = doc["results"][0];
  JsonObject results_0_now = results_0["now"];
  const char* results_0_now_text = results_0_now["text"]; // "Sunny"
  const char* results_0_now_code = results_0_now["code"]; // "0"
  const char* results_0_now_temperature = results_0_now["temperature"]; // "32"
  results_0_now_code_int = results_0_now["code"].as<int>(); 
  results_0_now_temperature_int = results_0_now["temperature"].as<int>(); 
  
  String results_0_last_update_str = results_0["last_update"].as<String>();   
}

void time_n()//用于中断判断
{
  n=1;
}

void weather_j1()//用于中断判断
{
  jj=1;
}
void oled_start()
{
  // u8g2.begin();//启动oled
  // u8g2.clearBuffer();					//清屏
  // u8g2.enableUTF8Print();//enable UTF8
  // u8g2.setFont(u8g2_font_wqy16_t_gb2312b);//设置中文字符集
  // u8g2.setCursor(32, 32);
  // u8g2.print("连接中...");
  // delay(2000);
  // u8g2.sendBuffer();					// transfer internal memory to the display
  // u8g2.clearBuffer();					//清屏
}
void oled_show_weather()
{
  u8g2.enableUTF8Print();//enable UTF8
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);//设置中文字符集
   //天气情况*********************************************
  switch(results_0_now_code_int)
  {
    case 0 : 
      u8g2.setCursor(98,58);
      u8g2.print("晴");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 1 : 
      u8g2.setCursor(98,58);
      u8g2.print("晴");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 4 :
      u8g2.setCursor(90,58);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 5 : 
      u8g2.setCursor(90,58);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 6 : 
      u8g2.setCursor(90,58);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 7 : 
      u8g2.setCursor(90,58);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 8 : 
      u8g2.setCursor(90,58);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 9 : 
      u8g2.setCursor(94,58);
      u8g2.print("阴");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 10 : 
      u8g2.setCursor(90,58);
      u8g2.print("阵雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 11 : 
      u8g2.setCursor(90,58);
      u8g2.print("雷雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 13 : 
      u8g2.setCursor(90,58);
      u8g2.print("小雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 14 : 
      u8g2.setCursor(90,58);
      u8g2.print("中雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 15 : 
      u8g2.setCursor(90,58);
      u8g2.print("大雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);
      break;

    case 16 : 
      u8g2.setCursor(90,58);
      u8g2.print("暴雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 17 : 
      u8g2.setCursor(90,58);
      u8g2.print("暴雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 18 : 
      u8g2.setCursor(90,58);
      u8g2.print("暴雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 19 : 
      u8g2.setCursor(90,58);
      u8g2.print("冻雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 20 : 
      u8g2.setCursor(90,58);
      u8g2.print("雨雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 21 : 
      u8g2.setCursor(90,58);
      u8g2.print("阵雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 22 : 
      u8g2.setCursor(90,58);
      u8g2.print("小雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 23 : 
      u8g2.setCursor(90,58);
      u8g2.print("中雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 24 : 
      u8g2.setCursor(90,58);
      u8g2.print("大雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    case 25 : 
      u8g2.setCursor(90,58);
      u8g2.print("暴雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;

    default : 
      u8g2.setCursor(90,58);
      u8g2.print("不明");
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8g2.drawGlyph(63, 60, 65);	
      break;
  }
  // 温度************************
  u8g2.setFont(u8g2_font_logisoso16_tf);
  u8g2.setCursor(8, 60);
  u8g2.print(results_0_now_temperature_int);	
  u8g2.print("°C");	
}
void drawTime() {
  // Display current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    //获取时间报错
    u8g2.clearBuffer();
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);
    // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setFont(u8g2_font_wqy16_t_gb2312b);
    u8g2.setCursor(25, 20);
    u8g2.print("未能获取时间");
    u8g2.setCursor(25, 40);
    u8g2.print("第");
    u8g2.print(timenum);
    u8g2.print("次尝试");
    timenum=timenum+1;
    u8g2.sendBuffer();
    delay(1500);
    return;
  }
  time_ok=time_ok+1;
  if(time_ok==1){
    //获取时间提示
    u8g2.clearBuffer();
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);
    // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setFont(u8g2_font_wqy16_t_gb2312b);
    u8g2.setCursor(25, 20);
    u8g2.print("更新时间成功");
    u8g2.setCursor(25, 40);
    u8g2.print("请稍后...");
    u8g2.sendBuffer();
    delay(500);
  }
  //绘制时间
  char timeString[16]; // Buffer big enough for "HH:MM:SS AM/PM"
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
  // u8g2.clearBuffer();
  // u8g2.setFontMode(1);  // Transparent
  // u8g2.setFontDirection(0);
  u8g2.setFont(u8g2_font_inb16_mn);  // Example font for time display
  u8g2.setCursor(8, 30);
  u8g2.print(timeString);
}
void draw_temp(){
   //绘制温湿度
  u8g2.clearBuffer();
  u8g2.setFontMode(1);  // Transparent
  u8g2.setFontDirection(0);
  // u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.setFont(u8g2_font_wqy15_t_gb2312);
  u8g2.drawUTF8(10, 25, "室内温度: ");  
  // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setCursor(80, 25);
  temptest = temptest +1;
  u8g2.print(temptest);
  u8g2.print("°C");	
  u8g2.drawUTF8(10, 55, "室内湿度: ");  
  u8g2.setCursor(80, 55);
  u8g2.print(temptest); 
  u8g2.print("RH"); 
  u8g2.sendBuffer();
}
void show_line()
{
  //外边框**********************
  u8g2.drawLine(0,63,128,63);
  u8g2.drawLine(0,0,128,0);
  u8g2.drawLine(127,0,127,64);
  u8g2.drawLine(0,0,0,64);
}
void setup()
{
  Serial.begin(115200);//串口通信
  // // pinMode(LED_BUILTIN, OUTPUT);//led使能
  // WiFi.begin(ssid, password);//wifi连接
  // //wifi函数
  // while ( WiFi.status() != WL_CONNECTED ) {
  //   delay (500 );
  //   Serial.print ( "." );
  // }
  delay(100);
  // Initialize display
  u8g2.begin();
  u8g2.enableUTF8Print();
  // Connect to Wi-Fi u8g2
  //WIFI正在连接提示
  u8g2.clearBuffer();
  u8g2.setFontMode(1);  // Transparent
  u8g2.setFontDirection(0);
  // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFont(u8g2_font_wqy15_t_gb2312);
  u8g2.setCursor(5, 20);
  u8g2.print("正在连接到");
  u8g2.setCursor(5, 40);
  u8g2.print(ssid);
  u8g2.sendBuffer();
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // u8g2.clearBuffer();
  //WIFI连接显示
  u8g2.setFontMode(1);  // Transparent
  u8g2.setFontDirection(0);
  // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);
  u8g2.setCursor(5, 60);
  u8g2.print("WiFi已连接");
  u8g2.sendBuffer();
  delay(1000);
  // Init time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  oled_start();
  weather_j1();//启动后获得天气
}
void loop() 
{ 
    static unsigned long lasttime = 0;  // 记录上次切换显示的时间
    static bool showWeather = false;             // 标志位，表示当前是否显示温湿度信息
    if(jj==1)//获取天气
    {
      get_weather();
      jj=0;
    }
    //oled*************************************************
    u8g2.clearBuffer();	//清屏

    if (millis() - lasttime >= 8000) {
      lasttime=millis();
      draw_temp();
      delay(2000);  // 显示2秒   
    }		
    else {
      oled_show_weather();
      u8g2.drawLine(0,40,127,40);
      show_line();
      drawTime();
    }
    u8g2.sendBuffer();	
    // delay(3000); 		
    // drawTime();
    // delay(1000); 				// transfer internal memory to the display
    //led();//led闪烁
}

