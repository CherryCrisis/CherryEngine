using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		Transform transform;
		Vector3 maxPosition = new Vector3();
		int step = 0;
		float speed = 0.01f;
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) 
			: base(cPtr, cMemoryOwn)
		{

		}

		public void Start()
		{
			transform = GetComponent<Transform>();
		}


		public void Update()
		{
			Debug.GetInstance().Info(transform);
			if (transform == null)
				return;
			
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float sensitityY = Time.GetInstance().GetDeltaTime() * deltaMouse.y;  

			float dt = Time.GetDeltaTime();

			float sign = 1;

			if (InputManager.GetKey(Keycode.W))
			{
				sign = 1;
				Debug.GetInstance().Log(ELogType.WARNING, "Code Wfgko Coucou Bem mets nous 20/20 (stp)");
			}
			else if (InputManager.GetKey(Keycode.S))
				sign = -1;
			else
				sign = 0;

			float x = sign * transform.Forward().x * speed * dt;
			float y = sign * transform.Forward().y * speed * dt;
			float z = sign * transform.Forward().z * speed * dt;
			transform.SetPosition(transform.position +  new Vector3(x,y,z));



			//if (transform == null)
			//	return;
			//
			//Vector2 deltaMouse = InputManager.GetMouseDelta();
			//float sensitityY = Time.GetInstance().GetDeltaTime() * deltaMouse.y;
			//
			//double angle = transform.eulerAngles.x + sensitityY;
			//angle = Math.Min(Math.Max(angle, -Math.PI * 0.4f), Math.PI * 0.4f);
			//
			//transform.eulerAngles = new Vector3((float)angle, transform.eulerAngles.y, transform.eulerAngles.z);
		}
	}
}
