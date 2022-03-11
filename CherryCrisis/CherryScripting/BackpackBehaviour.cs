using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using CCEngine;

namespace CCScripting
{
	class BackpackBehaviour : Behaviour
	{
		Transform transform;

		BackpackBehaviour(System.IntPtr cPtr, bool cMemoryOwn)
			: base(cPtr, cMemoryOwn)
		{
			transform = GetComponent<Transform>();
		}

		public void Start()
		{
		}

		float deltaTime = 0.01f;
		float time = 0f;
		public void Update()
		{
			time += 0.01f;

			transform.eulerAngles = new Vector3(transform.eulerAngles.x, CherryEngine.Sin(time), transform.eulerAngles.z);

			float timeScaledSpeed = deltaTime * 2f;

			if (InputManager.GetInstance().GetKey(Keycode.SPACE))
				transform.position = new Vector3(transform.position.x, transform.position.y + timeScaledSpeed, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.LEFT_CONTROL))
				transform.position = new Vector3(transform.position.x, transform.position.y - timeScaledSpeed, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.Z))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z + timeScaledSpeed);

			if (InputManager.GetInstance().GetKey(Keycode.S))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z - timeScaledSpeed);

			if (InputManager.GetInstance().GetKeyDown(Keycode.ESCAPE))
				Debug.GetInstance().Log(transform.position, transform.eulerAngles, transform.scale);

			transform.position.x = 5f;

			if (InputManager.GetInstance().GetKeyUp(Keycode.ESCAPE))
				Debug.GetInstance().Log(this);

		}
	}
}