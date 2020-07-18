#ifndef SensorReading_h
#define SensorReading_h

class SensorReading
{
  public:
    SensorReading();
    SensorReading(int type);
    SensorReading(int type, float value);
    SensorReading(int type, float value, int source);
    SensorReading(int type, float value, int source, float value_parsed);
    void post();
    void display();
    int _type;
    float _value;
    String _source;
    float _value_parsed;
    int _boot;
  private:
    int _id;
};
#endif
