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





//***************************修改二***************************
//使用i2c驱动，可以修改成自己使用的端口
// #define SCL 14
// #define SDA 12
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//注意屏幕方向
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R2, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);//oled显示
//U8G2_R2为屏幕旋转180
//U8G2_R0为屏幕不旋转
//*********************************************************

int n=0;//判断时间刷新标志
int jj=0;//天气更新标志

int currentYear;
int currentMonth;
int monthDay;
int weekDay;
int currentHour;
int currentMinute;
int currentSecon;

int results_0_now_code_int;//天气代码
int results_0_now_temperature_int;//温度

WiFiUDP ntpUDP;

//开启ntp时间协议
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com");

const char* host = "api.seniverse.com";     // 将要连接的服务器地址  
const int httpPort = 80;                    // 将要连接的服务器端口      



//*******************************修改三******************************
//私钥：心知天气官网注册账号免费使用，将自己的私钥复制
// 心知天气HTTP请求所需信息
String reqUserKey = "SguN9Q5nx54zA-Kg4";   // 私钥：心知天气官网注册账号免费使用，将自己的私钥复制
String reqLocation = "TaiYuan";            // 城市，改成你想要获取信息的城市英文
String reqUnit = "c";                      // 摄氏/华氏
//*****************************************************************



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
  const char* results_0_last_update = results_0["last_update"]; // "2020-06-02T14:40:00+08:00" 
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

void get_time_1()//获取时间
{
  weekDay = timeClient.getDay();
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();
  currentSecon = timeClient.getSeconds();
}
void get_date()//获取日期
{
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  currentYear = ptm->tm_year+1900;//年
  currentMonth = ptm->tm_mon+1;//月
  monthDay = ptm->tm_mday;//天
}

// void led()//led闪烁 1s一周期
// {
//   digitalWrite(LED_BUILTIN, LOW);
//   delay(500);
//   digitalWrite(LED_BUILTIN, HIGH);
//   delay(500);
// }
void oled_start()
{
  u8g2.begin();//启动oled
  u8g2.clearBuffer();					//清屏
  u8g2.enableUTF8Print();//enable UTF8
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);//设置中文字符集
  u8g2.setCursor(32, 32);
  u8g2.print("连接中...");
  delay(2000);
  u8g2.sendBuffer();					// transfer internal memory to the display
  u8g2.clearBuffer();					//清屏
}

void oled_show_weather()
{
  u8g2.enableUTF8Print();//enable UTF8
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);//设置中文字符集

   //天气情况*********************************************
  switch(results_0_now_code_int)
  {
    case 0 : 
      u8g2.setCursor(46,35);
      u8g2.print("晴");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 69);	
      break;

    case 1 : 
      u8g2.setCursor(46,35);
      u8g2.print("晴");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 69);	
      break;

    case 4 :
      u8g2.setCursor(38,35);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 65);	
      break;

    case 5 : 
      u8g2.setCursor(38,35);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 65);	
      break;

    case 6 : 
      u8g2.setCursor(38,35);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 65);	
      break;

    case 7 : 
      u8g2.setCursor(38,35);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 65);	
      break;

    case 8 : 
      u8g2.setCursor(38,35);
      u8g2.print("多云");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 65);	
      break;

    case 9 : 
      u8g2.setCursor(42,35);
      u8g2.print("阴");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 64);	
      break;

    case 10 : 
      u8g2.setCursor(38,35);
      u8g2.print("阵雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 11 : 
      u8g2.setCursor(38,35);
      u8g2.print("雷雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 13 : 
      u8g2.setCursor(38,35);
      u8g2.print("小雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 14 : 
      u8g2.setCursor(38,35);
      u8g2.print("中雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 15 : 
      u8g2.setCursor(38,35);
      u8g2.print("大雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);
      break;

    case 16 : 
      u8g2.setCursor(38,35);
      u8g2.print("暴雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 17 : 
      u8g2.setCursor(38,35);
      u8g2.print("暴雨");\
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 18 : 
      u8g2.setCursor(38,35);
      u8g2.print("暴雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 19 : 
      u8g2.setCursor(38,35);
      u8g2.print("冻雨");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 20 : 
      u8g2.setCursor(38,35);
      u8g2.print("雨雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 21 : 
      u8g2.setCursor(38,35);
      u8g2.print("阵雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 22 : 
      u8g2.setCursor(38,35);
      u8g2.print("小雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 23 : 
      u8g2.setCursor(38,35);
      u8g2.print("中雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 24 : 
      u8g2.setCursor(38,35);
      u8g2.print("大雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    case 25 : 
      u8g2.setCursor(38,35);
      u8g2.print("暴雪");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 67);	
      break;

    default : 
      u8g2.setCursor(38,35);
      u8g2.print("不明");
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(3, 34, 68);	
      break;
  }

  // 温度************************
  u8g2.setFont(u8g2_font_logisoso16_tf);
  u8g2.setCursor(2, 58);
  u8g2.print(results_0_now_temperature_int);
  u8g2.print("°C");	
}

void oled_show_time()
{
   u8g2.enableUTF8Print();//enable UTF8
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);//设置中文字符集

  //年月日***************************
  u8g2.setCursor(46,14);//年
  u8g2.print(currentYear);

  u8g2.setCursor(78,14);//间隔符/
  u8g2.print("/");

  if(currentMonth<10)//月
  {
    u8g2.drawStr(86,14,"0");
    u8g2.setCursor(94,14);
    u8g2.print(currentMonth);
  }
  else
  {
    u8g2.setCursor(86,14);
    u8g2.print(currentMonth);
  }

  u8g2.setCursor(102,14);//间隔符/
  u8g2.print("/");

  if(monthDay<10)//日
  {
    u8g2.drawStr(110,14,"0");
    u8g2.setCursor(118,14);
    u8g2.print(monthDay);
  }
  else
  {
    u8g2.setCursor(110,14);
    u8g2.print(monthDay);
  }

  //星期**********************
  u8g2.setCursor(88,35);
  u8g2.print("周");
  switch(weekDay)
  {
    case 1:
      u8g2.setCursor(104,35);
      u8g2.print("一");
    break;
    
    case 2:
      u8g2.setCursor(104,35);
      u8g2.print("二");
    break;
    
    case 3:
      u8g2.setCursor(104,35);
      u8g2.print("三");
    break;
    
    case 4:
      u8g2.setCursor(104,35);
      u8g2.print("四");
    break;
    
    case 5:
      u8g2.setCursor(104,35);
      u8g2.print("五");
    break;
    
    case 6:
      u8g2.setCursor(104,35);
      u8g2.print("六");
    break;
    
    case 0:
      u8g2.setCursor(104,35);
      u8g2.print("日");
    break;
  }

  u8g2.setFont(u8g2_font_logisoso16_tf); //设置字体
  //时间*************************
  if(currentHour<10)//小时
  {
    u8g2.drawStr(43,62,"0");
    u8g2.setCursor(51,62);
    u8g2.print(currentHour);
  }
  else
  {
    u8g2.setCursor(43,62);
    u8g2.print(currentHour);
  }

    u8g2.setCursor(66,62);//冒号：
    u8g2.print(":");

  if(currentMinute<10)//分钟
  {
    u8g2.drawStr(75,62,"0");
    u8g2.setCursor(83,62);
    u8g2.print(currentMinute);
  }
  else
  {
    u8g2.setCursor(75,62);
    u8g2.print(currentMinute);
  }

    u8g2.setCursor(96,62);//冒号：
    u8g2.print(":");

  if(currentSecon<10)//秒
  {
    u8g2.drawStr(107,62,"0");
    u8g2.setCursor(115,62);
    u8g2.print(currentSecon);
  }
  else
  {
    u8g2.setCursor(107,62);
    u8g2.print(currentSecon);
  }
}

void oled_show_line()
{
    //画内部线*******************
  u8g2.drawLine(38,0,38,16);
  u8g2.drawLine(72,16,72,42);
  u8g2.drawLine(38,42,38,64);
  u8g2.drawLine(38,16,72,16);
  u8g2.drawLine(38,42,72,42);
  u8g2.drawLine(38,42,72,42);
 
  //外边框**********************
  u8g2.drawLine(0,63,128,63);
  u8g2.drawLine(0,0,128,0);
  u8g2.drawLine(127,0,127,64);
  u8g2.drawLine(0,0,0,64);
}


void setup()
{
  Serial.begin(115200);//串口通信

  // pinMode(LED_BUILTIN, OUTPUT);//led使能

  WiFi.begin(ssid, password);//wifi连接

  //wifi函数
  while ( WiFi.status() != WL_CONNECTED ) {
    delay (500 );
    Serial.print ( "." );
  }
  oled_start();
  
  timeClient.begin();//获取时间初始化
  timeClient.setTimeOffset(28800);
  timeClient.update();
  
  weather_j1();//启动后获得天气
  get_date();//启动后获得日期
  get_time_1();//启动后获得时间

  timer1.attach_ms(500,time_n);//中断1：更新时间 1s 一次
  timer2.attach(600,weather_j1);//中断2：更新天气 10min 一次
}

void loop() 
{
  if(currentHour>23||currentHour<6)
  {
    u8g2.clearBuffer();					//清屏
    u8g2.sendBuffer();					// transfer internal memory to the display
    delay(500);

    ESP.deepSleep(10 * 1000000);//10秒
  }
  else
  {
    if(n==1)//获取时间
    {
      get_date();
      get_time_1();
      n=0;  
    }
    if(jj==1)//获取天气
    {
      get_weather();
      timeClient.update();
      jj=0;
    }
    //oled*************************************************
    u8g2.clearBuffer();					//清屏

    oled_show_weather();
    oled_show_time();
    oled_show_line();

    u8g2.sendBuffer();					// transfer internal memory to the display
    //led();//led闪烁
  }
}
