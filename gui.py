import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import os

files = []

def select_files():
    global files
    files = filedialog.askopenfilenames(title="Select files to compress")

    listbox.delete(0, tk.END)
    for f in files:
        # Show just the clean file name in the UI, not the massive path
        listbox.insert(tk.END, os.path.basename(f))

    status.config(text=f"{len(files)} file(s) selected for compression", fg="lightgreen")


def compress():
    key = entry.get()
    if not files or not key:
        messagebox.showerror("Error", "Select files and enter a password first!")
        return

    status.config(text="Compressing... Please wait ⏳", fg="orange")
    root.update_idletasks()

    # 🔥 THE FIX: Extract the folder path and just the base filenames
    target_folder = os.path.dirname(files[0])
    exe_path = os.path.abspath("zipper.exe")
    base_names = [os.path.basename(f) for f in files]

    # Pass ONLY the base names to C++ so it doesn't bake massive paths into the metadata
    cmd = [exe_path, "1", str(len(base_names))] + base_names + [key]

    try:
        # Run C++ inside the target folder
        result = subprocess.run(cmd, capture_output=True, text=True, check=True, cwd=target_folder)
        status.config(text="Compression Done 🚀", fg="lightgreen")
        messagebox.showinfo("Result", result.stdout.strip())
    except FileNotFoundError:
        status.config(text="Error: zipper.exe missing", fg="red")
        messagebox.showerror("Critical Error", "Could not find 'zipper.exe'.")
    except subprocess.CalledProcessError as e:
        status.config(text="C++ Engine Crashed", fg="red")
        messagebox.showerror("Execution Error", f"Output: {e.stderr}")


def decompress():
    key = entry.get()
    if not key:
        messagebox.showerror("Error", "Enter your decryption password first!")
        return

    # This handles the file picking automatically! No need to use the top button.
    file = filedialog.askopenfilename(
        title="Select YZIP archive",
        filetypes=[("YZIP files", "*.yzip")]
    )

    if not file:
        return

    listbox.delete(0, tk.END)
    listbox.insert(tk.END, f"Target: {os.path.basename(file)}")
    
    status.config(text="Decompressing... Please wait ⏳", fg="orange")
    root.update_idletasks()

    target_folder = os.path.dirname(file)
    exe_path = os.path.abspath("zipper.exe")
    base_name = os.path.basename(file)

    # Pass ONLY the base name to C++
    cmd = [exe_path, "2", base_name, key]

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True, cwd=target_folder)
        status.config(text="Decompression Done ✅", fg="lightgreen")
        messagebox.showinfo("Extracted Files", f"{result.stdout.strip()}\n\nCheck your folder!")
    except FileNotFoundError:
        status.config(text="Error: zipper.exe missing", fg="red")
        messagebox.showerror("Critical Error", "Could not find 'zipper.exe'.")
    except subprocess.CalledProcessError as e:
        status.config(text="Decryption Failed", fg="red")
        messagebox.showerror("Execution Error", "Decryption failed! You either typed the wrong password, or the file is corrupted.")


# -------- UI -------- #

root = tk.Tk()
root.title("YZIP Compressor")
root.geometry("500x450")
root.configure(bg="#1e1e1e")

tk.Label(root, text="YZIP File Compressor", font=("Segoe UI", 20, "bold"), fg="white", bg="#1e1e1e").pack(pady=15)

tk.Button(root, text="1. Select Files (For Compression Only)", font=("Segoe UI", 10, "bold"), command=select_files, bg="#444", fg="white", width=35).pack(pady=5)

listbox = tk.Listbox(root, width=65, height=8, bg="#2b2b2b", fg="white", borderwidth=0, highlightthickness=1, highlightcolor="#4CAF50")
listbox.pack(pady=10)

tk.Label(root, text="2. Enter Encryption Password", font=("Segoe UI", 10), bg="#1e1e1e", fg="#ccc").pack(pady=2)

entry = tk.Entry(root, show="*", width=30, font=("Segoe UI", 12), justify="center")
entry.pack(pady=5)

tk.Button(root, text="COMPRESS FILES", font=("Segoe UI", 11, "bold"), command=compress, bg="#4CAF50", fg="white", width=20).pack(pady=10)

tk.Label(root, text="--- OR ---", bg="#1e1e1e", fg="#888").pack(pady=2)

tk.Button(root, text="DECOMPRESS AN ARCHIVE", font=("Segoe UI", 11, "bold"), command=decompress, bg="#2196F3", fg="white", width=25).pack(pady=5)

status = tk.Label(root, text="System Idle", font=("Segoe UI", 10, "italic"), fg="#888", bg="#1e1e1e")
status.pack(pady=10)

root.mainloop()