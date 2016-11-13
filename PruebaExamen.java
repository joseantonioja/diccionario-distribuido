/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package pruebaexamen;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;

/**
 *
 * @author jose
 */
public class PruebaExamen {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        String server = "127.0.0.1";
        int pto = 9999;
        Socket cl=null;
        InetAddress dir = InetAddress.getByName(server);
        cl = new Socket(dir, pto);
        BufferedReader bf = new BufferedReader(new InputStreamReader(System.in));
        DataOutputStream dos = new DataOutputStream(cl.getOutputStream());
        DataInputStream dis = new DataInputStream(cl.getInputStream());
        ByteArrayOutputStream baos = new ByteArrayOutputStream(3000*4);
        DataOutputStream dos2 = new DataOutputStream(baos);
        System.out.println("Escribe la operacion: (0: insertar, 1: borrar, 2: mostrar)");
        String op = bf.readLine();
        if(op.equals("0")){
            String palabra, significado;
            System.out.println("Palabra:");
            palabra = bf.readLine();
            System.out.println("Significado");
            significado = bf.readLine();
            System.out.println("\nEnviando operacion " + palabra.length() + " "+significado.length());
            dos2.writeInt(0);
            dos2.writeInt(palabra.length());
            dos2.writeInt(significado.length());
            dos2.write(palabra.getBytes());
            dos2.write(significado.getBytes());
            dos.write(baos.toByteArray());
            dos.flush();
            int answer = dis.readInt();
            System.out.println("\nRespuesta "+answer);
        }
        else if(op.equals("1")){
            System.out.println("Palabra a borrar");
            String palabra = bf.readLine();
            System.out.println("\nEnviando operacion "+palabra.length());
            dos2.writeInt(1);
            dos2.writeInt(palabra.length());
            dos2.write(palabra.getBytes());
            dos.write(baos.toByteArray());
            dos.flush();
            int answer = dis.readInt();
            System.out.println("\nRespuesta "+answer);
        }
        else if(op.equals("2")){
            System.out.println("Mostrando palabras...");
            dos2.writeInt(2);
            dos.write(baos.toByteArray());
            dos.flush();
            int number_palabras = dis.readInt();
            byte buffer[] = new byte[300];
            System.out.println("Leyendo "+number_palabras + "palabras");
            for(int i=0; i<number_palabras; i++){
                int n = dis.readInt();
                dis.read(buffer, 0, n);
                String palabra = new String(buffer, 0, n);
                n = dis.readInt();
                dis.read(buffer, 0, n);
                String significado = new String(buffer, 0, n);
                System.out.println(palabra + ": "+ significado);
            }
        }
        
        cl.close();
    }
}
