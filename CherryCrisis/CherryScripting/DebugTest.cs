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
		int oui = 6;

		int SuperOui { set => oui = (int)(value - 7f); get => oui + 6; }

		public string name = "Debug il sait pas quoi";
		public void Debugger()
		{
			Debug.GetInstance().Log(ELogType.ERROR, name);
		}
	}
}
