#ifndef SUNPOSITION
#define SUNPOSITION

    #define DATA_POINTS 10

    typedef struct Time
    {
        int day;
        int hour;
    };

    typedef struct PolarDirection
    {
        float azimuth;
        float elevation;
        Time time;
    };

    PolarDirection directions[DATA_POINTS]={
        {100.0, 45.0, {1, 8}},
        {110.0, 50.0, {1, 12}},
        {120.0, 55.0, {2, 10}},
        {130.0, 60.0, {3, 14}},
        {140.0, 30.0, {4, 6}},
        {150.0, 20.0, {5, 18}},
        {160.0, 25.0, {6, 9}},
        {170.0, 35.0, {7, 16}},
        {180.0, 40.0, {8, 20}},
        {190.0, 42.0, {9, 11}}
    };

    int toAbsoluteHour(Time t) {
        return t.day * 24 + t.hour;
    }
    
    PolarDirection closestDirection(Time currentTime, PolarDirection* d, int minHourDiff = 0){
        int target = toAbsoluteHour(currentTime);
        int minDiff = 9999;
        int minIndex = 0;
        for (int i = 0; i < DATA_POINTS; i++) {
            int candidate = toAbsoluteHour(d[i].time);
            int diff = abs(candidate - target);

            if (diff < minDiff and diff > minHourDiff) {
                minDiff = diff;
                minIndex = i;
            }
        }
        return d[minIndex];
    };

    PolarDirection linearInterpolation(Time currentTime, PolarDirection* d){
        PolarDirection d1 = closestDirection(currentTime, d);
        PolarDirection d2 = closestDirection(currentTime, d, abs(toAbsoluteHour(d1.time)-toAbsoluteHour(currentTime)));
        PolarDirection interpolation;
        float f = abs(toAbsoluteHour(d2.time) - toAbsoluteHour(currentTime)) / abs(toAbsoluteHour(d1.time) - toAbsoluteHour(d2.time));
        interpolation.azimuth = d1.azimuth*f + d2.azimuth*(1-f);
        interpolation.elevation = d1.elevation*f + d2.elevation*(1-f);
        interpolation.time = currentTime;
        return interpolation;
    };

    PolarDirection sunPosition(Time currentTime){
        int target = toAbsoluteHour(currentTime);
        int minDiff = 9999;
        int minIndex = 0;
        for (int i = 0; i < DATA_POINTS; i++) {
            int candidate = toAbsoluteHour(directions[i].time);
            int diff = abs(candidate - target);

            if (diff < minDiff) {
                minDiff = diff;
                minIndex = i;
            }
        }
        return closestDirection(currentTime, directions);
    };

#endif