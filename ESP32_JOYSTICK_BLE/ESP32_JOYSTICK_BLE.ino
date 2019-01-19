#include "BLEDevice.h"

//http://d.hatena.ne.jp/wakwak_koba/20181011
//https://github.com/wakwak-koba/ESP32_BLE_Arduino
//https://qiita.com/coppercele/items/3320df0d047deda4cb22
//https://qiita.com/coppercele/items/3320df0d047deda4cb22

static int GPIO_LED_VOLUMEUP = 2;
static int GPIO_LED_ENTER    = 0;
static uint16_t GATT_HID = 0x1812;
static BLEUUID GATT_HID_REPORT((unsigned short)0x2a4d);

static BLEAddress *pServerAddress = NULL;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(GATT_HID)) {
      advertisedDevice.getScan()->stop();

      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      Serial.print("found device:");
      Serial.println(pServerAddress->toString().c_str());
    }
  }
};

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
  int i;
  Serial.println("DATAAAAAAAAAAAAAAAAAAA");
   // *pDataはuint8_t*
     //   int data = *pData;
    for(i=0;i<length;i++){
      Serial.print(pData[i],HEX);
      Serial.print(" ");
    }
        Serial.println();

  
  //uint16_t value = pData[0] << 8 | pData[1];
}

void setup() {
  Serial.begin(115200);
  pinMode(GPIO_LED_VOLUMEUP, OUTPUT);
  pinMode(GPIO_LED_ENTER   , OUTPUT);

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

std::map<uint16_t, BLERemoteCharacteristic *> * pCharacteristicMap;

void loop() {
  static boolean connected = false;

  if(pServerAddress != NULL && !connected)
  {
    BLEClient* pClient = BLEDevice::createClient();
    pClient->connect(*pServerAddress);
    
    BLERemoteService* pRemoteService = pClient->getService(GATT_HID);
    if(pRemoteService)
    {
      pCharacteristicMap = pRemoteService->getCharacteristicsByHandle();
      for (auto itr : *pCharacteristicMap)
      {
        Serial.print(itr.second->toString().c_str());
        if(GATT_HID_REPORT.equals(itr.second->getUUID()) && itr.second->canNotify())
        {
          itr.second->registerForNotify(notifyCallback);
          Serial.print(" registered");
        }
        Serial.println();
      }

      for(int i=0; i<2; i++)
      {
        digitalWrite(GPIO_LED_VOLUMEUP, HIGH);
        digitalWrite(GPIO_LED_ENTER   , HIGH);
        delay(100);
        digitalWrite(GPIO_LED_VOLUMEUP, LOW);
        digitalWrite(GPIO_LED_ENTER   , LOW);
        delay(400);
      }

      connected = true;
    }
  }



/*

    if (connected) {
   //http://www.neilkolban.com/esp32/docs/cpp_utils/html/class_b_l_e_remote_characteristic.html
      for (auto itr : *pCharacteristicMap)
      {

           if(itr.second->canRead()) {
              uint32_t  value = itr.second->readUInt32();
              Serial.print("The characteristic value was: ");
              Serial.println(value,HEX);
            }
     }
   
  }
*/

  
}
