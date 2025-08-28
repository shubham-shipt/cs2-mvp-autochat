// Preloader
window.addEventListener('load', function() {
    const preloader = document.getElementById('preloader');
    setTimeout(() => {
        preloader.style.opacity = '0';
        setTimeout(() => {
            preloader.style.display = 'none';
        }, 500);
    }, 1000);
});

// Particles.js Configuration
particlesJS('particles-js', {
    particles: {
        number: {
            value: 80,
            density: {
                enable: true,
                value_area: 800
            }
        },
        color: {
            value: '#00ff41'
        },
        shape: {
            type: 'circle',
            stroke: {
                width: 0,
                color: '#000000'
            }
        },
        opacity: {
            value: 0.5,
            random: false,
            anim: {
                enable: false,
                speed: 1,
                opacity_min: 0.1,
                sync: false
            }
        },
        size: {
            value: 3,
            random: true,
            anim: {
                enable: false,
                speed: 40,
                size_min: 0.1,
                sync: false
            }
        },
        line_linked: {
            enable: true,
            distance: 150,
            color: '#00ff41',
            opacity: 0.4,
            width: 1
        },
        move: {
            enable: true,
            speed: 6,
            direction: 'none',
            random: false,
            straight: false,
            out_mode: 'out',
            bounce: false,
            attract: {
                enable: false,
                rotateX: 600,
                rotateY: 1200
            }
        }
    },
    interactivity: {
        detect_on: 'canvas',
        events: {
            onhover: {
                enable: true,
                mode: 'repulse'
            },
            onclick: {
                enable: true,
                mode: 'push'
            },
            resize: true
        },
        modes: {
            grab: {
                distance: 400,
                line_linked: {
                    opacity: 1
                }
            },
            bubble: {
                distance: 400,
                size: 40,
                duration: 2,
                opacity: 8,
                speed: 3
            },
            repulse: {
                distance: 200,
                duration: 0.4
            },
            push: {
                particles_nb: 4
            },
            remove: {
                particles_nb: 2
            }
        }
    },
    retina_detect: true
});

// Custom Cursor
const cursor = document.querySelector('.custom-cursor');
let mouseX = 0;
let mouseY = 0;

document.addEventListener('mousemove', (e) => {
    mouseX = e.clientX;
    mouseY = e.clientY;
    cursor.style.left = mouseX + 'px';
    cursor.style.top = mouseY + 'px';
    cursor.style.opacity = '1';
});

document.addEventListener('mouseleave', () => {
    cursor.style.opacity = '0';
});

// Terminal Animation
const commands = [
    'whoami',
    'ls -la',
    'cat /etc/passwd',
    'nmap -sS target.com',
    'searchsploit kali',
    'hydra -l admin -P passwords.txt ssh://192.168.1.1',
    'john --wordlist=/usr/share/wordlists/rockyou.txt hashes.txt',
    'nc -lvnp 4444',
    'msfconsole',
    'sqlmap -u "http://target.com/vuln.php?id=1" --dbs'
];

let currentCommand = 0;
let currentChar = 0;
let isTyping = false;

function typeCommand() {
    if (isTyping) return;
    
    isTyping = true;
    const commandText = document.getElementById('command-text');
    const command = commands[currentCommand];
    
    commandText.textContent = '';
    currentChar = 0;
    
    const typeInterval = setInterval(() => {
        commandText.textContent += command[currentChar];
        currentChar++;
        
        if (currentChar >= command.length) {
            clearInterval(typeInterval);
            setTimeout(() => {
                commandText.textContent = '';
                currentCommand = (currentCommand + 1) % commands.length;
                isTyping = false;
                setTimeout(typeCommand, 1000);
            }, 2000);
        }
    }, 100);
}

// Start terminal animation
setTimeout(typeCommand, 2000);

// Search Functionality
function searchWallpapers() {
    const searchInput = document.getElementById('searchInput');
    const searchTerm = searchInput.value.toLowerCase().trim();
    const wallpapers = document.querySelectorAll('.wallpaper');
    
    if (!searchTerm) {
        wallpapers.forEach(wallpaper => {
            wallpaper.style.display = 'block';
        });
        return;
    }
    
    wallpapers.forEach(wallpaper => {
        const img = wallpaper.querySelector('img');
        const imgSrc = img.src.toLowerCase();
        const imgAlt = img.alt.toLowerCase();
        
        // Extract number from image source
        const numberMatch = imgSrc.match(/\/(\d+)\.(png|jpg|svg)/);
        const imageNumber = numberMatch ? numberMatch[1] : '';
        
        if (imageNumber.includes(searchTerm) || 
            imgSrc.includes(searchTerm) || 
            imgAlt.includes(searchTerm)) {
            wallpaper.style.display = 'block';
            wallpaper.classList.add('slide-up');
        } else {
            wallpaper.style.display = 'none';
        }
    });
}

// Add enter key support for search
document.getElementById('searchInput').addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
        searchWallpapers();
    }
});

// Speech Recognition for Terminal
let recognition;
let isListening = false;

if ('webkitSpeechRecognition' in window || 'SpeechRecognition' in window) {
    const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
    recognition = new SpeechRecognition();
    
    recognition.continuous = false;
    recognition.interimResults = false;
    recognition.lang = 'en-US';
    
    recognition.onresult = function(event) {
        const command = event.results[0][0].transcript;
        document.getElementById('command-text').textContent = command;
        isListening = false;
        updateMicButton();
    };
    
    recognition.onerror = function(event) {
        console.error('Speech recognition error:', event.error);
        isListening = false;
        updateMicButton();
    };
    
    recognition.onend = function() {
        isListening = false;
        updateMicButton();
    };
}

function updateMicButton() {
    const micBtn = document.getElementById('mic-btn');
    if (isListening) {
        micBtn.style.background = 'rgba(255, 0, 0, 0.2)';
        micBtn.style.borderColor = '#ff0000';
    } else {
        micBtn.style.background = 'rgba(0, 255, 65, 0.1)';
        micBtn.style.borderColor = 'rgba(0, 255, 65, 0.3)';
    }
}

document.getElementById('mic-btn').addEventListener('click', function() {
    if (!recognition) {
        alert('Speech recognition not supported in this browser');
        return;
    }
    
    if (isListening) {
        recognition.stop();
    } else {
        isListening = true;
        updateMicButton();
        recognition.start();
    }
});

// Text-to-Speech for Terminal Output
document.getElementById('speak-btn').addEventListener('click', function() {
    const commandText = document.getElementById('command-text').textContent;
    
    if (!commandText.trim()) {
        const messages = [
            'Welcome to Kali Wall, the best Kali Linux wallpaper collection',
            'Download high quality wallpapers for your cybersecurity setup',
            'Created by Shubham Singh for the hacking community'
        ];
        const randomMessage = messages[Math.floor(Math.random() * messages.length)];
        speak(randomMessage);
    } else {
        speak('Command: ' + commandText);
    }
});

function speak(text) {
    if ('speechSynthesis' in window) {
        const utterance = new SpeechSynthesisUtterance(text);
        utterance.rate = 0.8;
        utterance.pitch = 0.8;
        utterance.volume = 0.8;
        speechSynthesis.speak(utterance);
    } else {
        console.log('Text-to-speech not supported');
    }
}

// Stats Animation
function animateStats() {
    const stats = [
        { id: 'total-downloads', target: 62, duration: 2000 },
        { id: 'live-users', target: 3, duration: 1500 },
        { id: 'active-users', target: 57, duration: 2500 }
    ];
    
    stats.forEach(stat => {
        const element = document.getElementById(stat.id);
        let current = 0;
        const increment = stat.target / (stat.duration / 50);
        
        const timer = setInterval(() => {
            current += increment;
            if (current >= stat.target) {
                current = stat.target;
                clearInterval(timer);
            }
            element.textContent = Math.floor(current);
        }, 50);
    });
}

// Lazy Loading Images
function setupLazyLoading() {
    const images = document.querySelectorAll('img[loading="lazy"]');
    
    if ('IntersectionObserver' in window) {
        const imageObserver = new IntersectionObserver((entries, observer) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    const img = entry.target;
                    img.classList.add('fade-in');
                    observer.unobserve(img);
                }
            });
        });
        
        images.forEach(img => imageObserver.observe(img));
    }
}

// Navigation Effects
document.querySelectorAll('.nav-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
        e.preventDefault();
        
        // Add click effect
        this.style.transform = 'scale(0.95)';
        setTimeout(() => {
            this.style.transform = '';
        }, 150);
        
        // Simulate navigation action
        const action = this.textContent.trim();
        switch(action) {
            case '🕒 Latest':
                alert('Showing latest wallpapers');
                break;
            case '🏆 Toplist':
                alert('Showing top downloaded wallpapers');
                break;
            case '🎲 Random':
                scrollToRandomWallpaper();
                break;
            case '⬆️ Upload':
                alert('Upload feature coming soon!');
                break;
            case '💬 Forums':
                alert('Forums opening soon!');
                break;
        }
    });
});

function scrollToRandomWallpaper() {
    const wallpapers = document.querySelectorAll('.wallpaper');
    const randomWallpaper = wallpapers[Math.floor(Math.random() * wallpapers.length)];
    randomWallpaper.scrollIntoView({ behavior: 'smooth', block: 'center' });
    
    // Highlight the selected wallpaper
    randomWallpaper.style.border = '3px solid #00ff41';
    setTimeout(() => {
        randomWallpaper.style.border = '1px solid rgba(0, 255, 65, 0.3)';
    }, 2000);
}

// Download Counter
let downloadCount = 62;

document.querySelectorAll('.download-btn').forEach(btn => {
    btn.addEventListener('click', function() {
        downloadCount++;
        document.getElementById('total-downloads').textContent = downloadCount;
        
        // Add download effect
        this.style.background = '#ff6b35';
        this.textContent = 'Downloaded!';
        
        setTimeout(() => {
            this.style.background = 'rgba(0, 255, 65, 0.9)';
            this.textContent = 'Download';
        }, 1500);
    });
});

// Keyboard Shortcuts
document.addEventListener('keydown', function(e) {
    // Ctrl + / to focus search
    if (e.ctrlKey && e.key === '/') {
        e.preventDefault();
        document.getElementById('searchInput').focus();
    }
    
    // Escape to clear search
    if (e.key === 'Escape') {
        const searchInput = document.getElementById('searchInput');
        searchInput.value = '';
        searchWallpapers();
        searchInput.blur();
    }
    
    // Arrow keys for navigation
    if (e.key === 'ArrowDown' && e.ctrlKey) {
        e.preventDefault();
        window.scrollBy(0, 300);
    }
    
    if (e.key === 'ArrowUp' && e.ctrlKey) {
        e.preventDefault();
        window.scrollBy(0, -300);
    }
});

// Dark Mode Toggle (Hidden Feature)
let isDarkMode = true;

document.addEventListener('keydown', function(e) {
    if (e.ctrlKey && e.shiftKey && e.key === 'L') {
        isDarkMode = !isDarkMode;
        document.body.style.filter = isDarkMode ? 'none' : 'invert(1) hue-rotate(180deg)';
    }
});

// Initialize Everything
document.addEventListener('DOMContentLoaded', function() {
    // Add loading animation to elements
    document.querySelectorAll('.wallpaper').forEach((wallpaper, index) => {
        wallpaper.classList.add('loading');
        setTimeout(() => {
            wallpaper.classList.remove('loading');
            wallpaper.classList.add('loaded');
        }, index * 100);
    });
    
    // Start stats animation
    setTimeout(animateStats, 1500);
    
    // Setup lazy loading
    setupLazyLoading();
    
    // Add smooth scrolling
    document.documentElement.style.scrollBehavior = 'smooth';
    
    console.log('🎯 KaliWall initialized successfully!');
    console.log('💡 Tip: Use Ctrl+/ to focus search, Escape to clear');
    console.log('🎨 Hidden: Ctrl+Shift+L to toggle light mode');
});

// Performance optimization
let ticking = false;

function updateCursor() {
    cursor.style.transform = `translate(${mouseX}px, ${mouseY}px)`;
    ticking = false;
}

document.addEventListener('mousemove', (e) => {
    mouseX = e.clientX;
    mouseY = e.clientY;
    
    if (!ticking) {
        requestAnimationFrame(updateCursor);
        ticking = true;
    }
});

// Error Handling for Images
document.querySelectorAll('img').forEach(img => {
    img.addEventListener('error', function() {
        this.style.display = 'none';
        const parent = this.closest('.wallpaper');
        if (parent) {
            parent.style.display = 'none';
        }
    });
});

// Social Media Integration
document.querySelectorAll('.icon').forEach(icon => {
    icon.addEventListener('click', function(e) {
        // Add click animation
        this.style.transform = 'scale(1.2)';
        setTimeout(() => {
            this.style.transform = '';
        }, 200);
    });
});

// Console Easter Eggs
console.log(`
    ██╗  ██╗ █████╗ ██╗     ██╗    ██╗ █████╗ ██╗     ██╗     
    ██║ ██╔╝██╔══██╗██║     ██║    ██║██╔══██╗██║     ██║     
    █████╔╝ ███████║██║     ██║ █╗ ██║███████║██║     ██║     
    ██╔═██╗ ██╔══██║██║     ██║███╗██║██╔══██║██║     ██║     
    ██║  ██╗██║  ██║███████╗╚███╔███╔╝██║  ██║███████╗███████╗
    ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚══════╝╚══════╝
    
    🔥 Best Kali Linux Wallpapers Collection
    👨‍💻 Created by: Shubham Singh
    🌐 GitHub: https://github.com/shubham-shipt
`);

console.log('🎮 Try these commands:');
console.log('- searchWallpapers() to search programmatically');
console.log('- animateStats() to replay stats animation');
console.log('- scrollToRandomWallpaper() for random wallpaper');