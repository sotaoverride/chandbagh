 struct FirmwareData{
          uint8_t minor;
          uint8_t major;
    };
    enum Request{
          FWRequest = 0x5A,
          SensorRequest,
    };
    enum Response{
          FWResponse = 0xCA,
          SensorResponse,
    };

