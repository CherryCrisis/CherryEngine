using CCEngine;

namespace CCScripting
{
	public class DebugTest : Behaviour
	{

		public DebugTest(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn)
		{
		}

		void Start()
        {

		}

		public string name = "Debug il sait pas quoi";
		public void Debugger()
		{
			Debug.GetInstance().Log(ELogType.ERROR, name);
		}
	}
}
