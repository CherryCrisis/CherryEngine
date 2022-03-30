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

		public string name = "Je suis un nom";
		public void Debugger()
		{
			Debug.GetInstance().Log(name);
		}
	}
}
