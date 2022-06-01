using CCEngine;
using System;

namespace CCScripting
{
	public class LightModifier : Behaviour
	{
		public LightModifier(System.IntPtr cPtr, bool cMemoryOwn)
			: base(cPtr, cMemoryOwn)
		{

		}

		LightComponent light;


		public void Awake()
		{
			light = GetBehaviour<LightComponent>();

		}


		public void Update()
		{
			light.SetOuterCutoff(((float)Time.GetElapsedTime())); 
		}
	}
}
