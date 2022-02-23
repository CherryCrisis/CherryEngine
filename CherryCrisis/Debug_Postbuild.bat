xcopy /s /y DLL\Externals x64\Debug
xcopy /s /y Assets x64\Assets\
xcopy /s /y Internal x64\Internal\





Color RandomColor()
{
	float randomR = GD.RandRange(0.0, 1.0);
	float randomG = GD.RandRange(0.0, 1.0);
	float randomB = GD.RandRange(0.0, 1.0);
	
	return new Color(randomR, randomG, randomB); 
}

if (pArea == Paddle.Instance || pArea == Paddle2.Instance)
{
	velocity.x *= -1;
	
	Paddle.Instance.Modulate = RandomColor();
	Paddle2.Instance.Modulate = RandomColor();
}


