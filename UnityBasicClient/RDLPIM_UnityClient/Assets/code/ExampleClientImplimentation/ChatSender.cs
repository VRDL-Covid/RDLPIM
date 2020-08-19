
using UnityEngine;
using UnityEngine.UI;

public class ChatSender : MonoBehaviour
{
    
    public InputField source;

    public void Send()
    {
        RDLPIM_Controller rdlController = RDLPIM_Controller.Instance;
        rdlController.SendChat(source.text);
    }
}
