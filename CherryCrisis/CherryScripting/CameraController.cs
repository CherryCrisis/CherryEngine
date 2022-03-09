using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CCEngine;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn) {  }

		Transform transform;

		public void Start()
        {
			transform = GetComponent<Transform>();
		}

		float i = 0;


		public void Update()
		{
			i += 0.01f;
			transform.eulerAngles = new Vector3(transform.eulerAngles.x, CherryEngine.Sin(i), transform.eulerAngles.z);

			if (InputManager.GetInstance().GetKey(Keycode.SPACE))
				transform.position = new Vector3(transform.position.x, transform.position.y + 2f, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.LEFT_CONTROL))
				transform.position = new Vector3(transform.position.x, transform.position.y - 2f, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.Z))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z + 2f);

			if (InputManager.GetInstance().GetKey(Keycode.S))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z - 2f);

			Debug.GetInstance().Log(" / ", transform.position, transform.eulerAngles);
			Debug.GetInstance().Log(transform.position, transform.eulerAngles);
		
		}
	}
}
