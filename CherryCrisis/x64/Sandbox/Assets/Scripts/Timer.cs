
class Timer
{
    public Timer(float time) { totalTime = time; }

    float elapsedTime = 0f;
    public float totalTime = 1f;

    public bool isRunning = true;
    public bool Finished => elapsedTime >= totalTime;

    public bool Tick(float dt) 
    {
        if (!isRunning)
            return false;

        if (Finished)
            isRunning = false;

        elapsedTime += dt;

        return Finished && isRunning;
    }

    public bool CheckAndReset()
    {
        bool value = Finished;
        if (Finished)
            Reset();

        return value;
    }

    public void Reset() 
    {
        isRunning = true;
        elapsedTime = 0f;
    }
}

