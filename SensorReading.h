#ifndef SensorReading_h
#define SensorReading_h

class SensorReading
{
  public:
    SensorReading();
    SensorReading(int type);
    SensorReading(int type, int value);
    SensorReading(int type, int value, int source);
    SensorReading(int type, int value, int source, float value_parsed);
    SensorReading(int type, int value, int source, float value_parsed, int boot);
    void post();
    void display();
    int _type;
    int _value;
    String _source;
    float _value_parsed;
    int _boot;
  private:
    int _id;
};
#endif
