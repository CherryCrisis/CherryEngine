﻿
class Timer
{
    public Timer(float time) { totalTime = time; }

    float elapsedTime = 0f;
    public float totalTime = 1f;

    public bool Finished => elapsedTime >= totalTime;

    public void Tick(float dt) 
    {
        elapsedTime += dt;
    }

    public bool CheckAndReset()
    {
        if (Finished)
            Reset();

        return Finished;
    }

    public void Reset() 
    {
        elapsedTime = 0f;
    }
}
