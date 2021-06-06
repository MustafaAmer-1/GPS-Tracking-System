package eg.edu.asu.eng.gpstrackingsystem;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

public class BluetoothThread extends Thread{
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    public static final int COORDINATE_UPDATE = 6;
    Handler handler;

    public BluetoothThread(BluetoothSocket socket, Handler handler){
        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;
        this.handler = handler;
        Log.i("[THREAD-CT]","Creating thread");
        try{
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();

        } catch(IOException e) {
            Log.e("[THREAD-CT]","Error:"+ e.getMessage());
        }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;

        try {
            mmOutStream.flush();
        } catch (IOException e) {
            return;
        }
        Log.i("[THREAD-CT]","IO's Created");

    }

    @Override
    public void run() {
        super.run();
        BufferedReader br = new BufferedReader(new InputStreamReader(mmInStream));
        Log.i("[THREAD-CT]","Start Reading");
        while(true){
            try{
                String resp = br.readLine();
                //Transfer these data to the main thread
                Message msg = new Message();
                msg.what = COORDINATE_UPDATE;
                msg.obj = resp;
                handler.sendMessage(msg);
            }catch(IOException e){
                break;
            }
        }
        Log.i("[THREAD-CT]","Reading Stoped");
    }

    public void write(byte[] bytes){
        try{
            Log.i("[THREAD-CT]", "Writting bytes");
            mmOutStream.write(bytes);

        }catch(IOException e){}
    }

    public void cancel(){
        try{
            mmSocket.close();
        }catch(IOException e){}
    }
}
