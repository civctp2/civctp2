import shutil

try:
    import org.sikuli.script.SikulixForJython
    from sikuli import *
except ImportError:
    print("No run by sikuli jython")

fn= inspect.getsourcefile(lambda:0)
path= os.path.dirname(os.path.abspath(fn))
bsfn= os.path.splitext(os.path.basename(fn))[0]
addImagePath(path)

def main():
    wait("ctp2start-scr.png", 100)
    click("ctp2new-game-btn.png")
    click("ctp2launch-btn.png")
    if exists("ctp2ctr-bar.png", 100):
        if waitVanish("ctp2progress-bar.png", 100): # control bar appears before progressbar vanishes
            wait("ctp2settler.png", 30)
            type("b")
            wait("ctp2city-build.png", 30)
            doubleClick(Pattern("ctp2city-build_warrior.png").similar(0.79))
            wait(Pattern("ctp2city-build-queue_warrior.png").similar(0.95), 30)
            click(Pattern("ctp2city-build_close-btn.png").similar(0.90))
            if exists("ctp2ctr-bar_city.png", 30):
                file = capture(SCREEN.getBounds())
                if file:
                    f= Finder(file) # http://doc.sikuli.org/finder.html
                    f.find("ctp2ctr-bar.png")
                    if not f.hasNext():
                        print("Pattern not found in screen shot: " + file)
                        exit(30)
                    shutil.move(file, bsfn + '.png')
                    exit(0)
        else:
            exit(20)
    else:
        exit(10)
    exit(99)

if __name__ == "__main__":
    main()
