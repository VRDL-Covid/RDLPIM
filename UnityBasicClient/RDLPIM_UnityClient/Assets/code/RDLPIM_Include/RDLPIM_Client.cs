using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using UnityEngine;

// State object for receiving data from remote device.  

public class StateObject
{
    // Client socket.  
    public Socket workSocket = null;
    // Size of receive buffer.  
    public const int BufferSize = 2048;
    // Receive buffer.  
    public byte[] buffer = new byte[BufferSize];
    // Received data string.  
    public StringBuilder sb = new StringBuilder();
}


public class DataRecievedEventArgs: EventArgs
{
    public byte[] Data { get; set; }
    public int bytes;
    public RDLPIM_FucntionCode FucntionCode { get; set; }
}


public enum RDLPIM_FucntionCode { None = 0, data, chat, push, pull, subscribe, unsubscribe, VOIP, Info, error, DEBUG }

public class RDLPIM_Client
{

    //This example: https://docs.microsoft.com/en-us/dotnet/framework/network-programming/asynchronous-client-socket-example
    //another example: https://docs.microsoft.com/en-us/dotnet/framework/network-programming/asynchronous-server-socket-example

 //public members
    //Endpoint Definition
    public int RDLPIM_port = 8000;
    public string RDLPIM_IP = "127.0.0.1";

    public delegate void DataRecievedEventHandler(object source, DataRecievedEventArgs args);
    public event DataRecievedEventHandler DataRecieved;

    public delegate void RDLPIM_ConnetionLostHandler(object source, EventArgs args);
    public event RDLPIM_ConnetionLostHandler RDLPIM_ConnectionLost;

    //private members

    private static RDLPIM_Client s_Instance = null;
    private Socket client = null;
    public byte[] recBuff = new byte[2048];
    public int bytes = 0;
    private String response = String.Empty;
    
    // ManualResetEvent instances signal completion.  
    private ManualResetEvent connectDone =
        new ManualResetEvent(false);
    private ManualResetEvent sendDone =
        new ManualResetEvent(false);
    private ManualResetEvent receiveDone =
        new ManualResetEvent(false);

    private int ConnectionAttempts = 0;
    private bool RDLcoupled = false;

//public Methods

    public void SetPort(int p) { RDLPIM_port = p; }
    public void SetIP(string ip) { RDLPIM_IP = ip; }
    public byte[] GetLastMessage() { return recBuff; }
    public static RDLPIM_Client GetInstance()
    {
        if (s_Instance == null)
        {
            s_Instance = new RDLPIM_Client();
        }
        return s_Instance;
    }

    public void Receive()
    {
        if (RDLcoupled)
        {
            try
            {
                // Create the state object.  
                StateObject state = new StateObject();
                state.workSocket = client;
                bytes = 0;
                // Begin receiving the data from the remote device.  

                client.BeginReceive(recBuff, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReceiveCallback), state);
            }
            catch (Exception e)
            {
                client = null;
                RDLcoupled = false;
                OnConnectionLost();
            }
        }
    }
    public void InitiConnection()
    {
        // Connect to a remote device.  

        if(client != null || ConnectionAttempts >= 3)
        {
            return;
        }

        try
        {
            //Request invitation to connect to RDLPIM
            byte[] connectionDeets = new byte[2048];
            IPAddress ipAddress;
            IPAddress.TryParse(RDLPIM_IP, out ipAddress);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, RDLPIM_port);


            client = new Socket(ipAddress.AddressFamily,
                SocketType.Stream, ProtocolType.Tcp);

            // Connect to RDLPIM and await connection details
            client.BeginConnect(remoteEP,
                new AsyncCallback(ConnectCallback), client);

            //TODO block here for now, bu this needs to go.
            if (!connectDone.WaitOne(1000))
            {
                Debug.Log("Unable to connect to RDLPIM " + ConnectionAttempts + " times");
                client = null;

                if (ConnectionAttempts < 3)
                {
                    ConnectionAttempts++;
                    Debug.Log("Retrying...");
                }
                else
                {
                    Debug.Log("Failed to connect to RDLPIM check its status...");
                }
                return;
            }

            //Recieve and deserialse connection details
            client.Receive(connectionDeets);
            ConnectionDetails deets = new ConnectionDetails();
            deets.Deserialise(connectionDeets);
            client.Shutdown(SocketShutdown.Both);
            client.Close();


            //make RDLPIM handshake
            remoteEP = new IPEndPoint(ipAddress, deets.GetPort());
            // Create a TCP/IP socket.  
            client = new Socket(ipAddress.AddressFamily,
                                SocketType.Stream, ProtocolType.Tcp);
            // Connect to the remote endpoint.  
            client.BeginConnect(remoteEP,
                new AsyncCallback(ConnectCallback), client);
            if (!connectDone.WaitOne(200))
            {
                Debug.Log("RDLPIM Handshake failed");


                client = null;
                return;
            }
            RDLcoupled = true;

        }
        catch (Exception e)
        {
            
        }
    }

    public void RetryConnection()
    {
        ConnectionAttempts = 0;
        client = null;
        InitiConnection();
    }

    protected virtual void OnDataRecieved()
    {
        //split multiple recieved mesages
        int t_bytes = this.bytes;
        int offset = 0;


        while((offset+ BitConverter.ToInt32(recBuff, 4 + offset)) < t_bytes)
        {
            int partSize =  BitConverter.ToInt32(recBuff, 4);
            byte[] partialData = new byte[partSize];

            for (int i = 0; i < partSize; i++)
            {
                partialData[i] = recBuff[offset + 8 + i];
            }

            if (DataRecieved != null)
            {
                DataRecieved(this, new DataRecievedEventArgs { Data = partialData, bytes = partSize, FucntionCode = (RDLPIM_FucntionCode)BitConverter.ToInt32(recBuff, offset) });
            }

            offset += partSize+8;
        }
    }

    protected virtual void OnConnectionLost()
    {
        if(RDLPIM_ConnectionLost != null)
        {
            RDLPIM_ConnectionLost(this, new EventArgs());
        }
    }


    //privateMethods

    private void ConnectCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.  
            Socket client = (Socket)ar.AsyncState;

            // Complete the connection.  
            client.EndConnect(ar);

            Debug.Log("Socket connected to"+ 
                client.RemoteEndPoint.ToString());

            // Signal that the connection has been made.  
            connectDone.Set();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private void ReceiveCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the state object and the client socket
            // from the asynchronous state object.  
            StateObject state = (StateObject)ar.AsyncState;
            //Socket client = state.workSocket;

            // Read data from the remote device.  
            int bytesRead = client.EndReceive(ar);
            bytes += bytesRead;
            
            if (bytesRead > 0)
            {
                // There might be more data, so store the data received so far.  
                state.sb.Append(Encoding.ASCII.GetString(recBuff, 0, bytesRead));

                // Get the rest of the data.  
                client.BeginReceive(recBuff, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReceiveCallback), state);
            }
            else
            {
                // All the data has arrived; put it in response.  
                if (state.sb.Length > 1)
                {
                    response = state.sb.ToString(); 
                }
                // Signal that all bytes have been received.  
              
                receiveDone.Set();
            }

            // TODO - raise event.
            OnDataRecieved();
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
            RDLcoupled = false;
            //client = null;
            OnConnectionLost();
        }
    }

    public void Send(byte[] data)
    {
        // Begin sending the data to the remote device.  
        if (client != null)
        {
            client.BeginSend(data, 0, data.Length, 0, new AsyncCallback(SendCallback), client);
            sendDone.WaitOne();
        }
    }

    private void SendCallback(IAsyncResult ar)
    {
        try
        {
            // Complete sending the data to the remote device.  
            int bytesSent = client.EndSend(ar);

           // Signal that all bytes have been sent.  
           sendDone.Set();
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }
    private RDLPIM_Client() { }
    ~RDLPIM_Client()
    {
        client.Shutdown(SocketShutdown.Both);
        client.Close();
    }

}